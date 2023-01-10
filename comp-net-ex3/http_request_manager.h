#pragma once

#include "http_request.h"
#include "http_response.h"
#include <sstream>
#include <array>

class HttpRequestManager {
public:
	static const array<string, 2> k_SupportedHttpVersions;
	static const array<string, 1> k_SupportedMediaTypes;
	static const array<HttpRequest::Method, 6> k_SupportedHttpMethods;

private:
	HttpRequest::Method parseMethod(const string& i_MethodStr);
	HttpRequest parseRequestLine(const string& i_RequestLine);
	void parseRequestHeaders(stringstream& io_IncomingMessageStream, HttpRequest& o_Request);
	bool isBadRequest(const HttpRequest& i_Request);
	bool isUnsupportedHttpVersion(const HttpRequest& i_Request);
	bool isMethodNotImplemented(const HttpRequest& i_Request);
	bool isUnsupportedMediaType(const HttpRequest& i_Request);
	bool isContentLengthNotProvided(const HttpRequest& i_Request);
	bool isContentTypeNotProvided(const HttpRequest& i_Request);

public:
	string getRequestBody(const string& i_IncomingMessage, int i_BodyStartIndex, int i_ContentLength);
	HttpRequest getRequest(const string& i_IncomingMessage);
	HttpResponse processRequest(const HttpRequest& i_Request);

};