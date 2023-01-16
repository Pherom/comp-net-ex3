#pragma once

#include <string>
#include <sstream>
#include <unordered_map>
#include <algorithm>

using namespace std;

class HttpRequest {

public:
	enum class Method {
		UNSPECIFIED, OPTIONS, GET, HEAD, POST, PUT, DEL, TRACE, PATCH, CONNECT
	};

	static const unordered_map<string, Method> k_MethodNameToMethod;
	static const unordered_map<Method, string> k_MethodToMethodName;

private:
	Method m_Method = Method::UNSPECIFIED;
	string m_RequestURI;
	string m_ProtocolVersion;
	string m_MessageBody;
	unordered_map<string, string> m_Headers;

public:
	HttpRequest(const Method& i_Method, const string& i_RequestURI, const string& i_ProtocolVersion) :
		m_Method(i_Method), m_RequestURI(i_RequestURI), m_ProtocolVersion(i_ProtocolVersion) {}
	HttpRequest(const Method& i_Method, const string& i_RequestURI, const string& i_ProtocolVersion, const unordered_map<string, string>& i_Headers) :
		m_Method(i_Method), m_RequestURI(i_RequestURI), m_ProtocolVersion(i_ProtocolVersion), m_Headers(i_Headers) {}
	HttpRequest(const Method& i_Method, const string& i_RequestURI, const string& i_ProtocolVersion, const unordered_map<string, string>& i_Headers, const string& i_MessageBody) :
		m_Method(i_Method), m_RequestURI(i_RequestURI), m_ProtocolVersion(i_ProtocolVersion), m_Headers(i_Headers), m_MessageBody(i_MessageBody) {}
	Method getMethod() const;
	string getRequestURI() const;
	string getProtocolVersion() const;
	string getMessageBody() const;
	void setMessageBody(const string& i_MessageBody);
	unordered_map<string, string> getHeaders() const;
	string getHeaderValue(const string& i_HeaderName) const;
	void setHeader(const string& i_HeaderName, const string& i_HeaderValue);
	bool headerExists(const string& i_HeaderName) const;
	bool headerExistsWithValue(const string& i_HeaderName, const string& i_HeaderValue) const;
	unordered_map<string, string> getQueryParameters() const;
	string getQueryParameterValue(const string& i_QueryParameterName);
	bool queryParameterExists(const string& i_QueryParameterName);
	bool queryParameterExistsWithValue(const string& i_QueryParameterName, const string& i_QueryParameterValue);
	string toString() const;
};
