#pragma once

#include <unordered_map>
#include <string>
#include <algorithm>
#include <sstream>

using namespace std;

class HttpResponse {

public:
	enum class StatusCode {
		//Informational (1xx):
		CONTINUE, SWITCHING_PROTOCOLS, PROCESSING,
		//Successful (2xx):
		OK, CREATED, ACCEPTED, NON_AUTHORITATIVE_INFORMATION, NO_CONTENT,
		RESET_CONTENT, PARTIAL_CONTENT, MULTI_STATUS, ALREADY_REPORTED,
		IM_USED,
		//Redirection (3xx):
		MULTIPLE_CHOICES, MOVED_PERMANENTLY, FOUND, SEE_OTHER, NOT_MODIFIED,
		USE_PROXY, SWITCH_PROXY, TEMPORARY_REDIRECT, PERMANENT_REDIRECT,
		//Client Error (4xx):
		BAD_REQUEST, UNAUTHORIZED, PAYMENT_REQUIRED, FORBIDDEN, NOT_FOUND,
		METHOD_NOT_ALLOWED, NOT_ACCEPTABLE, PROXY_AUTHENTICATION_REQUIRED,
		REQUEST_TIMEOUT, CONFLICT, GONE, LENGTH_REQUIRED, PRECONDITION_FAILED,
		PAYLOAD_TOO_LARGE, URI_TOO_LONG, UNSUPPORTED_MEDIA_TYPE, RANGE_NOT_SATISFIABLE,
		EXPECTATION_FAILED, IM_A_TEAPOT, MISDIRECT_REQUEST, UNPROCESSABLE_ENTITY, LOCKED,
		FAILED_DEPENDENCY, UNOREDERED_COLLECTION, UPGRADE_REQUIRED, PRECONDITION_REQUIRED,
		TOO_MANY_REQUESTS, REQUEST_HEADER_FIELDS_TOO_LONG, CONNECTION_CLOSED_WITHOUT_RESPONSE,
		UNAVAILABLE_FOR_LEGAL_REASONS, CLIENT_CLOSED_REQUEST,
		//Server Error (5xx):
		INTERNAL_SERVER_ERROR, NOT_IMPLEMENTED, BAD_GATEWAY, SERVICE_UNAVAILABLE,
		GATEWAY_TIMEOUT, HTTP_VERSION_NOT_SUPPORTED, VARIANT_ALSO_NEGOTIATES,
		INSUFFICIENT_STORAGE, LOOP_DETECTED, NOT_EXTENDED, NETWORK_AUTHENTICATION_REQUIRED,
		NETWORK_CONNECT_TIMEOUT_ERROR
	};

	static const unordered_map<StatusCode, pair<string, int>> k_SupportedStatusCodes;

private:
	string m_ProtocolVersion;
	StatusCode m_StatusCode;
	unordered_map<string, string> m_Headers;
	string m_MessageBody;

public:
	HttpResponse(const StatusCode& i_StatusCode, const string& i_ProtocolVersion) :
		m_StatusCode(i_StatusCode), m_ProtocolVersion(i_ProtocolVersion) {}
	HttpResponse(const StatusCode& i_StatusCode, const string& i_ProtocolVersion, const unordered_map<string, string>& i_Headers) :
		m_StatusCode(i_StatusCode), m_ProtocolVersion(i_ProtocolVersion), m_Headers(i_Headers) {}
	HttpResponse(const StatusCode& i_StatusCode, const string& i_ProtocolVersion, const unordered_map<string, string>& i_Headers, const string& i_MessageBody) :
		m_StatusCode(i_StatusCode), m_ProtocolVersion(i_ProtocolVersion), m_Headers(i_Headers), m_MessageBody(i_MessageBody) {}
	string getProtocolVersion() const;
	StatusCode getStatusCode() const;
	string getMessageBody() const;
	void setMessageBody(const string& i_MessageBody);
	unordered_map<string, string> getHeaders() const;
	string getHeaderValue(const string& i_HeaderName) const;
	void setHeader(const string& i_HeaderName, const string& i_HeaderValue);
	bool headerExists(const string& i_HeaderName);
	bool headerExistsWithValue(const string& i_HeaderName, const string& i_HeaderValue);
	string toString();
};