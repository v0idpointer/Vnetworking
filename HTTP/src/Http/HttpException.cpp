#include <Vnetworking/Http/HttpException.h>

#include <unordered_map>
#include <format>

using namespace Vnetworking::Http;

static const std::unordered_map<HttpErrorType, std::string> s_errorTypes = { 

	{ HttpErrorType::REQUEST_PARSING_ERROR, "HTTP request parsing error" },
	{ HttpErrorType::REQUEST_SERIALIZATION_ERROR, "HTTP request serialization error" },
	{ HttpErrorType::RESPONSE_PARSING_ERROR, "HTTP response parsing error" },
	{ HttpErrorType::RESPONSE_SERIALIZATION_ERROR, "HTTP response serialization error" },

};

static const std::unordered_map<HttpErrorSubtype, std::string> s_errorSubtypes = { 

	{ HttpErrorSubtype::GENERIC_ERROR, "An error has occurred." },
	{ HttpErrorSubtype::INVALID_STATUS_CODE, "Invalid HTTP status code." },
	{ HttpErrorSubtype::INVALID_HEADER_NAME, "Invalid HTTP header name." },
	{ HttpErrorSubtype::INVALID_HEADER_VALUE, "Invalid HTTP header value." },
	{ HttpErrorSubtype::INVALID_HTTP_VERSION, "Invalid and/or unsupported HTTP version." },
	
};

constexpr std::string_view ERR_BAD_ERROR_TYPE = "Invalid HTTP error type.";
constexpr std::string_view ERR_BAD_ERROR_SUBTYPE = "Invalid HTTP error subtype.";

std::string Vnetworking::Http::ToString(const HttpErrorType type) {
	if (!s_errorTypes.contains(type))
		throw std::invalid_argument(ERR_BAD_ERROR_TYPE.data());
	return s_errorTypes.at(type);
}

std::string Vnetworking::Http::ToString(const HttpErrorSubtype subtype) {
	if (!s_errorSubtypes.contains(subtype))
		throw std::invalid_argument(ERR_BAD_ERROR_SUBTYPE.data());
	return s_errorSubtypes.at(subtype);
}

static inline std::string CreateErrorMessage(const HttpErrorType type, const HttpErrorSubtype subtype) noexcept {
	return std::format("{0}: {1}", ToString(type), ToString(subtype));
}

static inline std::string CreateErrorMessage(const HttpErrorType type, const HttpErrorSubtype subtype, const std::string& message) {
	std::string subtypeText = ToString(subtype);
	subtypeText = subtypeText.substr(0, subtypeText.length() - 1);
	return std::format("{0}: {1}: {2}", ToString(type), subtypeText, message);
}

HttpException::HttpException(const HttpErrorType type, const HttpErrorSubtype subtype)
	: std::runtime_error(CreateErrorMessage(type, subtype)), m_errType(type), m_errSubtype(subtype) { }

HttpException::HttpException(const HttpErrorType type, const HttpErrorSubtype subtype, const std::string& message)
	: std::runtime_error(CreateErrorMessage(type, subtype, message)), m_errType(type), m_errSubtype(subtype) { }

HttpException::HttpException(const HttpException& other) noexcept
	: std::runtime_error(other.what()), m_errType(other.m_errType), m_errSubtype(other.m_errSubtype) { }

HttpException::~HttpException() { }

std::string HttpException::What() const {
	return this->what();
}

HttpErrorType HttpException::GetErrorType() const {
	return this->m_errType;
}

HttpErrorSubtype HttpException::GetErrorSubtype() const {
	return this->m_errSubtype;
}