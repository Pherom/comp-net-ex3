#include "http_response_builder.h"

HttpResponseBuilder HttpResponseBuilder::setStatusCode(const HttpResponse::StatusCode& i_StatusCode) {
	m_StatusCode = i_StatusCode;
	return *this;
}

HttpResponseBuilder HttpResponseBuilder::setProtocolVersion(const string& i_ProtocolVersion) {
	m_ProtocolVersion = i_ProtocolVersion;
	return *this;
}

HttpResponseBuilder HttpResponseBuilder::setHeader(const string& i_HeaderName, const string& i_HeaderValue) {
	string name = i_HeaderName, value = i_HeaderValue;
	transform(name.begin(), name.end(), name.begin(), ::tolower);
	transform(value.begin(), value.end(), value.begin(), ::tolower);
	m_Headers[name] = value;
	return *this;
}

HttpResponseBuilder HttpResponseBuilder::setMessageBody(const string& i_MessageBody) {
	stringstream newContentLengthStrStream;
	newContentLengthStrStream << i_MessageBody.length();
	setHeader("content-type", "text/html");
	setHeader("content-length", newContentLengthStrStream.str());
	m_MessageBody = i_MessageBody;
	return *this;
}

HttpResponse HttpResponseBuilder::build() {
	return HttpResponse(m_StatusCode, m_ProtocolVersion, m_Headers, m_MessageBody);
}