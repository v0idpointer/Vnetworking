#include <Vnetworking/Http/HttpStatusCode.h>

#include <string>
#include <format>
#include <unordered_map>
#include <exception>
#include <stdexcept>

using namespace Vnetworking::Http;

// https://developer.mozilla.org/en-US/docs/Web/HTTP/Status
static const std::unordered_map<HttpStatusCode, std::string> s_statusCodes = {

	{ HttpStatusCode::CONTINUE, "Continue" },
	{ HttpStatusCode::SWITCHING_PROTOCOLS, "Switching Protocols" },
	{ HttpStatusCode::PROCESSING, "Processing" },
	{ HttpStatusCode::EARLY_HINTS, "Early Hints" },

	{ HttpStatusCode::OK, "OK" },
	{ HttpStatusCode::CREATED, "Created" },
	{ HttpStatusCode::ACCEPTED, "Accepted" },
	{ HttpStatusCode::NON_AUTHORITATIVE_INFORMATION, "Non-Authoritative Information" },
	{ HttpStatusCode::NO_CONTENT, "No Content" },
	{ HttpStatusCode::RESET_CONTENT, "Reset Content" },
	{ HttpStatusCode::PARTIAL_CONTENT, "Partial Content" },
	{ HttpStatusCode::MULTI_STATUS, "Multi-Status" },
	{ HttpStatusCode::ALREADY_REPORTED, "Already Reported" },
	{ HttpStatusCode::IM_USED, "IM Used" },

	{ HttpStatusCode::MULTIPLE_CHOICES, "Multiple Choices" },
	{ HttpStatusCode::MOVED_PERMANENTLY, "Moved Permanently" },
	{ HttpStatusCode::FOUND, "Found" },
	{ HttpStatusCode::SEE_OTHER, "See Other" },
	{ HttpStatusCode::NOT_MODIFIED, "Not Modified" },
	{ HttpStatusCode::USE_PROXY, "Use Proxy" },
	{ HttpStatusCode::TEMPORARY_REDIRECT, "Temporary Redirect" },
	{ HttpStatusCode::PERMANENT_REDIRECT, "Permanent Redirect" },

	{ HttpStatusCode::BAD_REQUEST, "Bad Request" },
	{ HttpStatusCode::UNAUTHORIZED, "Unauthorized" },
	{ HttpStatusCode::PAYMENT_REQUIRED, "Payment Required" },
	{ HttpStatusCode::FORBIDDEN, "Forbidden" },
	{ HttpStatusCode::NOT_FOUND, "Not Found" },
	{ HttpStatusCode::METHOD_NOT_ALLOWED, "Method Not Allowed" },
	{ HttpStatusCode::NOT_ACCEPTABLE, "Not Acceptable" },
	{ HttpStatusCode::PROXY_AUTHENTICATION_REQUIRED, "Proxy Authentication Required" },
	{ HttpStatusCode::REQUEST_TIMEOUT, "Request Timeout" },
	{ HttpStatusCode::CONFLICT, "Conflict" },
	{ HttpStatusCode::GONE, "Gone" },
	{ HttpStatusCode::LENGTH_REQUIRED, "Length Required" },
	{ HttpStatusCode::PRECONDITION_FAILED, "Precondition Failed" },
	{ HttpStatusCode::PAYLOAD_TOO_LARGE, "Payload Too Large" },
	{ HttpStatusCode::URI_TOO_LONG, "URI Too Long" },
	{ HttpStatusCode::UNSUPPORTED_MEDIA_TYPE, "Unsupported Media Type" },
	{ HttpStatusCode::RANGE_NOT_SATISFIABLE, "Range Not Satisfiable" },
	{ HttpStatusCode::EXPECTATION_FAILED, "Expectation Failed" },
	{ HttpStatusCode::IM_A_TEAPOT, "I'm a teapot" },
	{ HttpStatusCode::MISDIRECTED_REQUEST, "Misdirected Request" },
	{ HttpStatusCode::UNPROCESSABLE_CONTENT, "Unprocessable Content" },
	{ HttpStatusCode::LOCKED, "Locked" },
	{ HttpStatusCode::FAILED_DEPENDENCY, "Failed Dependency" },
	{ HttpStatusCode::TOO_EARLY, "Too Early" },
	{ HttpStatusCode::UPGRADE_REQUIRED, "Upgrade Required" },
	{ HttpStatusCode::PRECONDITION_REQUIRED, "Precondition Required" },
	{ HttpStatusCode::TOO_MANY_REQUESTS, "Too Many Requests" },
	{ HttpStatusCode::REQUEST_HEADER_FIELDS_TOO_LARGE, "Request Header Fields Too Large" },
	{ HttpStatusCode::UNAVAILABLE_FOR_LEGAL_REASONS, "Unavailable For Legal Reasons" },

	{ HttpStatusCode::INTERNAL_SERVER_ERROR, "Internal Server Error" },
	{ HttpStatusCode::NOT_IMPLEMENTED, "Not Implemented" },
	{ HttpStatusCode::BAD_GATEWAY, "Bad Gateway" },
	{ HttpStatusCode::SERVICE_UNAVAILABLE, "Service Unavailable" },
	{ HttpStatusCode::GATEWAY_TIMEOUT, "Gateway Timeout" },
	{ HttpStatusCode::HTTP_VERSION_NOT_SUPPORTED, "HTTP Version Not Supported" },
	{ HttpStatusCode::VARIANT_ALSO_NEGOTIATES, "Variant Also Negotiates" },
	{ HttpStatusCode::INSUFFICIENT_STORAGE, "Insufficient Storage" },
	{ HttpStatusCode::LOOP_DETECTED, "Loop Detected" },
	{ HttpStatusCode::NOT_EXTENDED, "Not Extended" },
	{ HttpStatusCode::NETWORK_AUTHENTICATION_REQUIRED, "Network Authentication Required" },

};

std::string Vnetworking::Http::ToString(const HttpStatusCode status) {
	if (!s_statusCodes.contains(status)) 
		throw std::invalid_argument(std::format("{0} is not a valid HTTP status code.", static_cast<int>(status)));
	return s_statusCodes.at(status);
}