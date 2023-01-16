#pragma once

#include "http_response.h"
#include <string>

using namespace std;

class HttpResponseBuilder {

private:
	string m_ProtocolVersion = "HTTP/1.1";
	HttpResponse::StatusCode m_StatusCode = HttpResponse::StatusCode::OK;
	unordered_map<string, string> m_Headers = {
	{ "connection", "keep-alive" },
	{ "content-type", "text/html" },
	{ "content-length", "0" }
	};
	string m_MessageBody;

public:
	HttpResponseBuilder setStatusCode(const HttpResponse::StatusCode& i_StatusCode);
	HttpResponseBuilder setProtocolVersion(const string& i_ProtocolVersion);
	HttpResponseBuilder setHeader(const string& i_HeaderName, const string& i_HeaderValue);
	HttpResponseBuilder setMessageBody(const string& i_MessageBody);
	HttpResponse build();
};