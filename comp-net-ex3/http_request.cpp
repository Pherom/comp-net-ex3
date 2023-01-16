#include "http_request.h"

const unordered_map<string, HttpRequest::Method> HttpRequest::k_MethodNameToMethod = {
	{ "OPTIONS", Method::OPTIONS },
	{ "GET", Method::GET },
	{ "HEAD", Method::HEAD },
	{ "POST", Method::POST },
	{ "PUT", Method::PUT },
	{ "DELETE", Method::DEL },
	{ "TRACE", Method::TRACE },
	{ "PATCH", Method::PATCH },
	{ "CONNECT", Method::CONNECT }
};

const unordered_map<HttpRequest::Method, string> HttpRequest::k_MethodToMethodName = {
	{ Method::OPTIONS, "OPTIONS"},
	{ Method::GET, "GET" },
	{ Method::HEAD, "HEAD" },
	{ Method::POST, "POST" },
	{ Method::PUT, "PUT" },
	{ Method::DEL, "DELETE" },
	{ Method::TRACE, "TRACE" },
	{ Method::PATCH, "PATCH" },
	{ Method::CONNECT, "CONNECT" }
};

HttpRequest::Method HttpRequest::getMethod() const {
	return m_Method;
}

string HttpRequest::getRequestURI() const {
	return m_RequestURI;
}

string HttpRequest::getProtocolVersion() const {
	return m_ProtocolVersion;
}

string HttpRequest::getMessageBody() const {
	return m_MessageBody;
}

void HttpRequest::setMessageBody(const string& i_MessageBody) {
	stringstream newContentLengthStrStream;
	newContentLengthStrStream << i_MessageBody.length();
	setHeader("content-length", newContentLengthStrStream.str());
	m_MessageBody = i_MessageBody;
}

unordered_map<string, string> HttpRequest::getHeaders() const {
	return m_Headers;
}

string HttpRequest::getHeaderValue(const string& i_HeaderName) const {
	string name = i_HeaderName;
	transform(name.begin(), name.end(), name.begin(), ::tolower);
	return m_Headers.at(name);
}

void HttpRequest::setHeader(const string& i_HeaderName, const string& i_HeaderValue) {
	string name = i_HeaderName, value = i_HeaderValue;
	transform(name.begin(), name.end(), name.begin(), ::tolower);
	transform(value.begin(), value.end(), value.begin(), ::tolower);
	m_Headers[name] = value;
}

bool HttpRequest::headerExists(const string& i_HeaderName) const {
	string name = i_HeaderName;
	transform(name.begin(), name.end(), name.begin(), ::tolower);
	return m_Headers.count(name) > 0;
}

bool HttpRequest::headerExistsWithValue(const string& i_HeaderName, const string& i_HeaderValue) const {
	string value = i_HeaderValue;
	transform(value.begin(), value.end(), value.begin(), ::tolower);
	return headerExists(i_HeaderName) && getHeaderValue(i_HeaderName) == value;
}

unordered_map<string, string> HttpRequest::getQueryParameters() const {
	unordered_map<string, string> queryParameters;
	int endKeyValueIndex = m_RequestURI.find('?');
	string queryPart;
	if (endKeyValueIndex != string::npos) {
		queryPart = m_RequestURI.substr(endKeyValueIndex + 1);
		while (queryPart.length() > 0) {
			int keyValueSeparatorIndex = queryPart.find('=');
			endKeyValueIndex = queryPart.find('&');
			if (endKeyValueIndex == string::npos) {
				endKeyValueIndex = queryPart.length();
			}
			string key = queryPart.substr(0, keyValueSeparatorIndex);
			string value = queryPart.substr(keyValueSeparatorIndex + 1, endKeyValueIndex - keyValueSeparatorIndex - 1);
			queryParameters[key] = value;
			queryPart = queryPart.erase(0, endKeyValueIndex + 1);
		}
	}
	return queryParameters;
}

string HttpRequest::getQueryParameterValue(const string& i_QueryParameterName) {
	return getQueryParameters().at(i_QueryParameterName);
}

bool HttpRequest::queryParameterExists(const string& i_QueryParameterName) {
	return getQueryParameters().count(i_QueryParameterName) > 0;
}

bool HttpRequest::queryParameterExistsWithValue(const string& i_QueryParameterName, const string& i_QueryParameterValue) {
	unordered_map<string, string> queryParameters = getQueryParameters();
	return queryParameters.count(i_QueryParameterName) > 0 && queryParameters.at(i_QueryParameterName) == i_QueryParameterValue;
}

string HttpRequest::toString() const {
	stringstream requestStr;

	requestStr << k_MethodToMethodName.at(m_Method) << " " << m_RequestURI << " " << m_ProtocolVersion << "\r\n";
	for (auto header : m_Headers) {
		requestStr << header.first << ": " << header.second << "\r\n";
	}

	requestStr << m_MessageBody << "\r\n";

	return requestStr.str();
}