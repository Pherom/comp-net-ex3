#pragma once

#include "http_request.h"
#include "http_response.h"
#include "http_response_builder.h"
#include <sstream>
#include <array>
#include <fstream>
#include <filesystem>

using namespace std;

class HttpRequestManager {
public:
	static const array<string, 2> k_SupportedHttpVersions;
	static const string k_RootPath;

private:
	HttpRequest::Method parseMethod(const string& i_MethodStr);
	HttpRequest parseRequestLine(const string& i_RequestLine);
	void parseRequestHeaders(stringstream& io_IncomingMessageStream, HttpRequest& o_Request);
	bool isBadRequest(const HttpRequest& i_Request);
	bool isUnsupportedHttpVersion(const HttpRequest& i_Request);
	bool isUnsupportedMediaType(const HttpRequest& i_Request);
	bool isContentLengthNotProvided(const HttpRequest& i_Request);
	bool isContentTypeNotProvided(const HttpRequest& i_Request);
	string getPathFromURI(const HttpRequest& i_Request);
	HttpResponse processGetRequest(const HttpRequest& i_Request);
	HttpResponse processHeadRequest(const HttpRequest& i_Request);
	HttpResponse processOptionsRequest(const HttpRequest& i_Request);
	HttpResponse processPutRequest(const HttpRequest& i_Request);
	HttpResponse processPostRequest(const HttpRequest& i_Request);
	HttpResponse processTraceRequest(const HttpRequest& i_Request);
	HttpResponse processDeleteRequest(const HttpRequest& i_Request);

public:
	string getRequestBody(const string& i_IncomingMessage, int i_BodyStartIndex, int i_ContentLength);
	HttpRequest getRequest(const string& i_IncomingMessage);
	HttpResponse processRequest(const HttpRequest& i_Request);

};