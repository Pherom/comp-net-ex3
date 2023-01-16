#include "http_request_manager.h"
#include <iostream>

const array<string, 2> HttpRequestManager::k_SupportedHttpVersions { "http/1.0", "http/1.1" };
const string HttpRequestManager::k_RootPath = "./www";

HttpRequest::Method HttpRequestManager::parseMethod(const string& i_MethodStr) {
	HttpRequest::Method method = HttpRequest::Method::UNSPECIFIED;
	auto res = HttpRequest::k_MethodNameToMethod.find(i_MethodStr);
	if (res != HttpRequest::k_MethodNameToMethod.end()) {
		method = res->second;
	}
	return method;
}

HttpRequest HttpRequestManager::parseRequestLine(const string& i_RequestLine) {
	string methodStr, requestURI, protocolVersion;
	stringstream requestLineStream(i_RequestLine);
	requestLineStream >> methodStr >> requestURI >> protocolVersion;
	transform(protocolVersion.begin(), protocolVersion.end(), protocolVersion.begin(), ::tolower);
	return HttpRequest(parseMethod(methodStr), requestURI, protocolVersion);
}

void HttpRequestManager::parseRequestHeaders(stringstream& io_IncomingMessageStream, HttpRequest& o_Request) {
	string header;
	while (getline(io_IncomingMessageStream, header) && header != "\r") {
		int indexOfColon = header.find(':');
		int indexOfEnd = header.find('\r');
		string name = header.substr(0, indexOfColon);
		string value = header.substr(indexOfColon + 2, indexOfEnd - indexOfColon - 2);
		o_Request.setHeader(name, value);
	}
}

string HttpRequestManager::getRequestBody(const string& i_IncomingMessage, int i_BodyStartIndex, int i_ContentLength) {
	string messageBody;
	messageBody = i_IncomingMessage.substr(i_BodyStartIndex, i_ContentLength);
	return messageBody;
}

HttpRequest HttpRequestManager::getRequest(const string& i_IncomingMessage) {
	stringstream incomingMessageStream(i_IncomingMessage);
	string requestLine;

	getline(incomingMessageStream, requestLine);
	HttpRequest request = parseRequestLine(requestLine);

	parseRequestHeaders(incomingMessageStream, request);

	return request;
}

bool HttpRequestManager::isBadRequest(const HttpRequest& i_Request) {
	bool badRequest = false;

	if (i_Request.getMethod() == HttpRequest::Method::UNSPECIFIED) {
		badRequest = true;
	}
	else if (i_Request.getProtocolVersion().rfind("http/", 0) == string::npos) {
		badRequest = true;
	}
	else if (!i_Request.headerExists("Host")) {
		badRequest = true;
	}

	return badRequest;
}

bool HttpRequestManager::isUnsupportedHttpVersion(const HttpRequest& i_Request) {
	return find(k_SupportedHttpVersions.begin(), k_SupportedHttpVersions.end(), i_Request.getProtocolVersion()) == k_SupportedHttpVersions.end();
}

bool HttpRequestManager::isUnsupportedMediaType(const HttpRequest& i_Request) {
	string contentType = i_Request.getHeaderValue("content-type");
	string preSlash = contentType.substr(0, contentType.find('/'));
	return  preSlash != "*" && preSlash != "text";
}

bool HttpRequestManager::isContentLengthNotProvided(const HttpRequest& i_Request) {
	return !i_Request.headerExists("Content-Length");
}

bool HttpRequestManager::isContentTypeNotProvided(const HttpRequest& i_Request) {
	return !i_Request.headerExists("Content-Type");
}

string HttpRequestManager::getPathFromURI(const HttpRequest& i_Request) {
	string path;
	int queryStart = i_Request.getRequestURI().find('?');
	unordered_map<string, string> queryParams = i_Request.getQueryParameters();
	bool specificLang = (i_Request.getMethod() == HttpRequest::Method::GET || i_Request.getMethod() == HttpRequest::Method::HEAD) && queryParams.count("lang") > 0;
	bool isDirectory;

	if (queryStart != string::npos) {
		path = k_RootPath + i_Request.getRequestURI().substr(0, queryStart);
	}

	else {
		path = k_RootPath + i_Request.getRequestURI();
	}

	isDirectory = i_Request.getRequestURI().find('.') == string::npos;

	if (isDirectory) {
		if (specificLang) {
			path += "/lang/" + queryParams.at("lang");
		}

		path += "/index.html";
	}

	else {
		if (specificLang) {
			int indexOfFileSeparator = path.find_last_of('/');
			path = path.insert(indexOfFileSeparator, "/lang/" + queryParams.at("lang"));
		}
	}

	return path;
}

HttpResponse HttpRequestManager::processGetRequest(const HttpRequest& i_Request) {
	HttpResponseBuilder builder;
	string path = getPathFromURI(i_Request);

	ifstream file(path);

	if (!file.good()) {
		builder.setStatusCode(HttpResponse::StatusCode::NOT_FOUND);
	}

	else {
		stringstream buffer;
		buffer << file.rdbuf();
		builder.setMessageBody(buffer.str());
	}

	file.close();
	return builder.build();
}

HttpResponse HttpRequestManager::processHeadRequest(const HttpRequest& i_Request) {
	HttpResponseBuilder builder;
	string path = getPathFromURI(i_Request);

	ifstream file(path);

	if (!file.good()) {
		builder.setStatusCode(HttpResponse::StatusCode::NOT_FOUND);
	}

	else {
		stringstream buffer, lengthBuffer;
		buffer << file.rdbuf();
		lengthBuffer << buffer.str().length();
		builder.setHeader("content-length", lengthBuffer.str());
	}

	file.close();
	return builder.build();
}

HttpResponse HttpRequestManager::processOptionsRequest(const HttpRequest& i_Request) {
	HttpResponseBuilder builder;
	string path = getPathFromURI(i_Request);
	stringstream allowHeaderValueStream;

	allowHeaderValueStream << HttpRequest::k_MethodToMethodName.at(HttpRequest::Method::OPTIONS)
		<< ", " << HttpRequest::k_MethodToMethodName.at(HttpRequest::Method::TRACE) << ", " << HttpRequest::k_MethodToMethodName.at(HttpRequest::Method::PUT);

	if (filesystem::exists(path)) {
		allowHeaderValueStream << ", " << HttpRequest::k_MethodToMethodName.at(HttpRequest::Method::GET)
			<< ", " << HttpRequest::k_MethodToMethodName.at(HttpRequest::Method::HEAD) << ", " << HttpRequest::k_MethodToMethodName.at(HttpRequest::Method::DEL);
	}

	else {
		allowHeaderValueStream << ", " << HttpRequest::k_MethodToMethodName.at(HttpRequest::Method::POST);
	}

	builder.setHeader("allow", allowHeaderValueStream.str());
	builder.setHeader("accept", "text/*");

	return builder.build();
}

HttpResponse HttpRequestManager::processPutRequest(const HttpRequest& i_Request) {
	HttpResponseBuilder builder;

	if (isContentTypeNotProvided(i_Request)) {
		builder.setStatusCode(HttpResponse::StatusCode::BAD_REQUEST);
	}

	else if (isContentLengthNotProvided(i_Request)) {
		builder.setStatusCode(HttpResponse::StatusCode::LENGTH_REQUIRED);
	}

	else if (isUnsupportedMediaType(i_Request)) {
		builder.setStatusCode(HttpResponse::StatusCode::UNSUPPORTED_MEDIA_TYPE);
	}

	else {
		string path = getPathFromURI(i_Request);
		if (!filesystem::exists(path)) {
			builder.setStatusCode(HttpResponse::StatusCode::CREATED);
		}
		ofstream file(path);
		file << i_Request.getMessageBody();
		cout << i_Request.getMessageBody() << endl;
		file.close();
	}

	return builder.build();
}

HttpResponse HttpRequestManager::processPostRequest(const HttpRequest& i_Request) {
	HttpResponseBuilder builder;

	if (isContentTypeNotProvided(i_Request)) {
		builder.setStatusCode(HttpResponse::StatusCode::BAD_REQUEST);
	}

	else if (isContentLengthNotProvided(i_Request)) {
		builder.setStatusCode(HttpResponse::StatusCode::LENGTH_REQUIRED);
	}

	else if (isUnsupportedMediaType(i_Request)) {
		builder.setStatusCode(HttpResponse::StatusCode::UNSUPPORTED_MEDIA_TYPE);
	}

	else {
		string path = getPathFromURI(i_Request);
		if (!filesystem::exists(path)) {
			builder.setStatusCode(HttpResponse::StatusCode::CREATED);
			ofstream file(path);
			file << i_Request.getMessageBody();
			cout << "Request Manager: POST request produced file with contents:" << endl;
			string line;
			stringstream messageBodyStream(i_Request.getMessageBody());
			while (getline(messageBodyStream, line)) {
				cout << "\t" << line << endl;
			}
			file.close();
		}

		else {
			builder.setStatusCode(HttpResponse::StatusCode::CONFLICT);
		}
	}

	return builder.build();
}

HttpResponse HttpRequestManager::processTraceRequest(const HttpRequest& i_Request) {
	HttpResponseBuilder builder;

	if (filesystem::exists(getPathFromURI(i_Request))) {
		for (auto header : i_Request.getHeaders()) {
			builder.setHeader(header.first, header.second);
		}

		builder.setMessageBody(i_Request.getMessageBody());
	}

	else {
		builder.setStatusCode(HttpResponse::StatusCode::NOT_FOUND);
	}

	return builder.build();
}

HttpResponse HttpRequestManager::processDeleteRequest(const HttpRequest& i_Request) {
	HttpResponseBuilder builder;
	string path = getPathFromURI(i_Request);

	if (!filesystem::exists(path)) {
		builder.setStatusCode(HttpResponse::StatusCode::NOT_FOUND);
	}

	else {
		filesystem::remove(path);
	}

	return builder.build();
}

HttpResponse HttpRequestManager::processRequest(const HttpRequest& i_Request) {
	cout << "\tRequest Manager: Processing " << HttpRequest::k_MethodToMethodName.at(i_Request.getMethod()) << " request with URI: " << i_Request.getRequestURI() << endl;
	HttpResponseBuilder builder;

	if (isBadRequest(i_Request)) {
		builder.setStatusCode(HttpResponse::StatusCode::BAD_REQUEST);
	}
	else if (isUnsupportedHttpVersion(i_Request)) {
		builder.setStatusCode(HttpResponse::StatusCode::HTTP_VERSION_NOT_SUPPORTED);
	}
	else {
		switch(i_Request.getMethod()) {
		case HttpRequest::Method::GET:
			return processGetRequest(i_Request);
		case HttpRequest::Method::HEAD:
			return processHeadRequest(i_Request);
		case HttpRequest::Method::POST:
			return processPostRequest(i_Request);
		case HttpRequest::Method::PUT:
			return processPutRequest(i_Request);
		case HttpRequest::Method::DEL:
			return processDeleteRequest(i_Request);
		case HttpRequest::Method::TRACE:
			return processTraceRequest(i_Request);
		case HttpRequest::Method::OPTIONS:
			return processOptionsRequest(i_Request);
		default:
			builder.setStatusCode(HttpResponse::StatusCode::NOT_IMPLEMENTED);
			break;
		}
	}

	return builder.build();
}

//HttpResponse HttpRequestManager::processRequest(const HttpRequest& i_Request) {
//	HttpResponse response(HttpResponse::StatusCode::OK, "HTTP/1.1");
//	string body;
//	stringstream bodyStream;
//	bodyStream << "Received an " << i_Request.getProtocolVersion() << " " << HttpRequest::k_MethodToMethodName.at(i_Request.getMethod()) << " request to: " << i_Request.getRequestURI() << endl;
//	bodyStream << "with Headers:" << endl;
//	for (auto header : i_Request.getHeaders()) {
//		bodyStream << header.first << ": " << header.second << endl;
//	}
//	bodyStream << "with Query Parameters:" << endl;
//	for (auto queryParam : i_Request.getQueryParameters()) {
//		bodyStream << queryParam.first << " = " << queryParam.second << endl;
//	}
//	bodyStream << "with Body:" << endl;
//	bodyStream << i_Request.getMessageBody();
//	body = bodyStream.str();
//	stringstream bodyLengthStringStream;
//	bodyLengthStringStream << body.length();
//	return HttpResponse(HttpResponse::StatusCode::OK, "HTTP/1.1", { { "content-length", bodyLengthStringStream.str() } }, body);
//}