#include "http_response.h"

const unordered_map<HttpResponse::StatusCode, pair<string, int>> HttpResponse::k_SupportedStatusCodes = {
	//Informational (1xx):
	{ StatusCode::CONTINUE, { "Continue", 100 }},
	{ StatusCode::SWITCHING_PROTOCOLS, { "Switching Protocols", 101 }},
	{ StatusCode::PROCESSING, { "Processing", 102 }},
	//Successful (2xx):
	{ StatusCode::OK, { "OK", 200 }},
	{ StatusCode::CREATED, { "Created", 201 }},
	{ StatusCode::ACCEPTED, { "Accepted", 202 }},
	{ StatusCode::NON_AUTHORITATIVE_INFORMATION, { "Non-Authoritative Information", 203 }},
	{ StatusCode::NO_CONTENT, { "No Content", 204 }},
	{ StatusCode::RESET_CONTENT, { "Reset Content", 205 }},
	{ StatusCode::PARTIAL_CONTENT, { "Partial Content", 206 }},
	{ StatusCode::MULTI_STATUS, { "Multi-Status", 207 }},
	{ StatusCode::ALREADY_REPORTED, { "Already Reported", 208 }},
	{ StatusCode::IM_USED, { "IM Used", 226 }},
	//Redirection (3xx):
	{ StatusCode::MULTIPLE_CHOICES, { "Multiple Choices", 300 }},
	{ StatusCode::MOVED_PERMANENTLY, { "Moved Permanently", 301 }},
	{ StatusCode::FOUND, { "Found", 302 }},
	{ StatusCode::SEE_OTHER, { "See Other", 303 }},
	{ StatusCode::NOT_MODIFIED, { "Not Modified", 304 }},
	{ StatusCode::USE_PROXY, { "Use Proxy", 305 }},
	{ StatusCode::SWITCH_PROXY, { "Switch Proxy", 306 }},
	{ StatusCode::TEMPORARY_REDIRECT, { "Temporary Redirect", 307 }},
	{ StatusCode::PERMANENT_REDIRECT, { "Permanent Redirect", 308 }},
	//Client Error (4xx):
	{ StatusCode::BAD_REQUEST, { "Bad Request", 400 }},
	{ StatusCode::UNAUTHORIZED, { "Unauthorized", 401 }},
	{ StatusCode::PAYMENT_REQUIRED, { "Payment Required", 402 }},
	{ StatusCode::FORBIDDEN, { "Forbidden", 403 }},
	{ StatusCode::NOT_FOUND, { "Not Found", 404 }},
	{ StatusCode::METHOD_NOT_ALLOWED, { "Method Not Allowed", 405 }},
	{ StatusCode::NOT_ACCEPTABLE, { "Not Acceptable", 406 }},
	{ StatusCode::PROXY_AUTHENTICATION_REQUIRED, { "Proxy Authentication Required", 407 }},
	{ StatusCode::REQUEST_TIMEOUT, { "Request Timeout", 408 }},
	{ StatusCode::CONFLICT, { "Conflict", 409 }},
	{ StatusCode::GONE, { "Gone", 410 }},
	{ StatusCode::LENGTH_REQUIRED, { "Length Required", 411 }},
	{ StatusCode::PRECONDITION_FAILED, { "Precondition Failed", 412 }},
	{ StatusCode::PAYLOAD_TOO_LARGE, { "Payload Too Large", 413 }},
	{ StatusCode::URI_TOO_LONG, { "URI Too Long", 414 }},
	{ StatusCode::UNSUPPORTED_MEDIA_TYPE, { "Unsupported Media Type", 415 }},
	{ StatusCode::RANGE_NOT_SATISFIABLE, { "Range Not Satisfiable", 416 }},
	{ StatusCode::EXPECTATION_FAILED, { "Expectation Failed", 417 }},
	{ StatusCode::IM_A_TEAPOT, { "I'm a teapot", 418 }},
	{ StatusCode::MISDIRECT_REQUEST, { "Misdirected Request", 421 }},
	{ StatusCode::UNPROCESSABLE_ENTITY, { "Unprocessable Entity", 422 }},
	{ StatusCode::LOCKED, { "Locked", 423 }},
	{ StatusCode::FAILED_DEPENDENCY, { "Failed Dependency", 424 }},
	{ StatusCode::UNOREDERED_COLLECTION, { "Unordered Collection", 425 }},
	{ StatusCode::UPGRADE_REQUIRED, { "Upgrade Required", 426 }},
	{ StatusCode::PRECONDITION_REQUIRED, { "Precondition Required", 428 }},
	{ StatusCode::TOO_MANY_REQUESTS, { "Too Many Requests", 429 }},
	{ StatusCode::REQUEST_HEADER_FIELDS_TOO_LONG, { "Request Header Fields Too Large", 431 }},
	{ StatusCode::CONNECTION_CLOSED_WITHOUT_RESPONSE, { "Connection Closed Without Response", 444 }},
	{ StatusCode::UNAVAILABLE_FOR_LEGAL_REASONS, { "Unavailable For Legal Reasons", 451 }},
	{ StatusCode::CLIENT_CLOSED_REQUEST, { "Client Closed Request", 499 }},
	//Server Error (5xx):
	{ StatusCode::INTERNAL_SERVER_ERROR, { "Internal Server Error", 500 }},
	{ StatusCode::NOT_IMPLEMENTED, { "Not Implemented", 501 }},
	{ StatusCode::BAD_GATEWAY, { "Bad Gateway", 502 }},
	{ StatusCode::SERVICE_UNAVAILABLE, { "Service Unavailable", 503 }},
	{ StatusCode::GATEWAY_TIMEOUT, { "Gateway Timeout", 504 }},
	{ StatusCode::HTTP_VERSION_NOT_SUPPORTED, { "HTTP Version Not Supported", 505 }},
	{ StatusCode::VARIANT_ALSO_NEGOTIATES, { "Variant Also Negotiates", 506 }},
	{ StatusCode::INSUFFICIENT_STORAGE, { "Insufficient Storage", 507 }},
	{ StatusCode::LOOP_DETECTED, { "Loop Detected", 508 }},
	{ StatusCode::NOT_EXTENDED, { "Not Extended", 510 }},
	{ StatusCode::NETWORK_AUTHENTICATION_REQUIRED, { "Network Authentication Required", 511 }},
	{ StatusCode::NETWORK_CONNECT_TIMEOUT_ERROR, { "Network Connect Timeout Error", 599 }},
};

string HttpResponse::getProtocolVersion() const {
	return m_ProtocolVersion;
}

HttpResponse::StatusCode HttpResponse::getStatusCode() const {
	return m_StatusCode;
}

string HttpResponse::getMessageBody() const {
	return m_MessageBody;
}

void HttpResponse::setMessageBody(const string& i_MessageBody) {
	stringstream newContentLengthStrStream;
	newContentLengthStrStream << i_MessageBody.length();
	setHeader("Content-Length", newContentLengthStrStream.str());
	m_MessageBody = i_MessageBody;
}

unordered_map<string, string> HttpResponse::getHeaders() const {
	return m_Headers;
}

string HttpResponse::getHeaderValue(const string& i_HeaderName) const {
	string name = i_HeaderName;
	transform(name.begin(), name.end(), name.begin(), ::tolower);
	return m_Headers.at(name);
}

void HttpResponse::setHeader(const string& i_HeaderName, const string& i_HeaderValue) {
	string name = i_HeaderName, value = i_HeaderValue;
	transform(name.begin(), name.end(), name.begin(), ::tolower);
	transform(value.begin(), value.end(), value.begin(), ::tolower);
	m_Headers[name] = value;
}

bool HttpResponse::headerExists(const string& i_HeaderName) {
	string name = i_HeaderName;
	transform(name.begin(), name.end(), name.begin(), ::tolower);
	return m_Headers.count(name) > 0;
}

bool HttpResponse::headerExistsWithValue(const string& i_HeaderName, const string& i_HeaderValue) {
	string value = i_HeaderValue;
	transform(value.begin(), value.end(), value.begin(), ::tolower);
	return headerExists(i_HeaderName) && getHeaderValue(i_HeaderName) == value;
}

string HttpResponse::toString() {
	stringstream respStrStream;
	respStrStream << m_ProtocolVersion << " " << k_SupportedStatusCodes.at(m_StatusCode).second << " " << k_SupportedStatusCodes.at(m_StatusCode).first << "\r\n";
	
	for (auto header : m_Headers) {
		respStrStream << header.first << ": " << header.second << "\r\n";
	}

	respStrStream << "\r\n" << m_MessageBody;

	return respStrStream.str();
}