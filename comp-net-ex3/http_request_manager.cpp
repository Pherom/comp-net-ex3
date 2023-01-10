#include "http_request_manager.h"

const array<string, 2> HttpRequestManager::k_SupportedHttpVersions { "http/1.0", "http/1.1" };
const array<string, 1> k_SupportedMediaTypes { "text/html" };
const array<HttpRequest::Method, 6> k_SupportedHttpMethods {
	HttpRequest::Method::GET,
	HttpRequest::Method::POST,
	HttpRequest::Method::PUT,
	HttpRequest::Method::OPTIONS,
	HttpRequest::Method::TRACE,
	HttpRequest::Method::DEL
};

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
		stringstream headerStream(header);
		string name;
		getline(headerStream, name, ':');
		string value;
		getline(headerStream, value);
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

bool HttpRequestManager::isMethodNotImplemented(const HttpRequest& i_Request) {
	return find(k_SupportedHttpMethods.begin(), k_SupportedHttpMethods.end(), i_Request.getMethod()) == k_SupportedHttpMethods.end();
}

bool HttpRequestManager::isUnsupportedMediaType(const HttpRequest& i_Request) {
	return find(k_SupportedMediaTypes.begin(), k_SupportedMediaTypes.end(), i_Request.getHeaderValue("Content-Type")) == k_SupportedMediaTypes.end();
}

bool HttpRequestManager::isContentLengthNotProvided(const HttpRequest& i_Request) {
	return !i_Request.headerExists("Content-Length");
}

bool HttpRequestManager::isContentTypeNotProvided(const HttpRequest& i_Request) {
	return !i_Request.headerExists("Content-Type");
}

HttpResponse HttpRequestManager::processRequest(const HttpRequest& i_Request) {
	HttpResponse::StatusCode respStatusCode;
	
	if (isBadRequest(i_Request)) {
		respStatusCode = HttpResponse::StatusCode::BAD_REQUEST;
	}
	else if (isUnsupportedHttpVersion(i_Request)) {
		respStatusCode = HttpResponse::StatusCode::HTTP_VERSION_NOT_SUPPORTED;
	}
	else if (isMethodNotImplemented(i_Request)) {
		respStatusCode = HttpResponse::StatusCode::NOT_IMPLEMENTED;
	}
	else {
		if (i_Request.getMethod() == HttpRequest::Method::POST || i_Request.getMethod() == HttpRequest::Method::PUT) {
			if (isContentTypeNotProvided(i_Request)) {
				respStatusCode = HttpResponse::StatusCode::BAD_REQUEST;
			}

			else if (isContentLengthNotProvided(i_Request)) {
				respStatusCode = HttpResponse::StatusCode::LENGTH_REQUIRED;
			}

			else if (isUnsupportedMediaType(i_Request)) {
				respStatusCode = HttpResponse::StatusCode::UNSUPPORTED_MEDIA_TYPE;
			}
		}
	}
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