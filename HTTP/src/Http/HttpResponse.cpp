#include <Vnetworking/Http/HttpResponse.h>
#include <Vnetworking/Http/HttpException.h>

#include <sstream>
#include <format>
#include <algorithm>

using namespace Vnetworking::Http;

HttpResponse::HttpResponse() 
	: HttpResponse(HttpStatusCode::OK) { }

HttpResponse::HttpResponse(const std::uint32_t statusCode) 
	: HttpResponse(static_cast<HttpStatusCode>(statusCode)) { }

HttpResponse::HttpResponse(const HttpStatusCode statusCode) {
	this->m_statusCode = statusCode;
	this->m_headers = { };
	this->m_payload = { };
}

HttpResponse::HttpResponse(const HttpResponse& httpResponse) {
	this->operator= (httpResponse);
}

HttpResponse::HttpResponse(HttpResponse&& httpResponse) noexcept {
	this->operator= (std::move(httpResponse));
}

HttpResponse::~HttpResponse() { }

HttpResponse& HttpResponse::operator= (const HttpResponse& httpResponse) {

	this->m_statusCode = httpResponse.m_statusCode;
	this->m_headers = httpResponse.m_headers;
	this->m_payload = { httpResponse.m_payload.begin(), httpResponse.m_payload.end() };

	return static_cast<HttpResponse&>(*this);
}

HttpResponse& HttpResponse::operator= (HttpResponse&& httpResponse) noexcept {

	this->m_statusCode = httpResponse.m_statusCode;
	this->m_headers = std::move(httpResponse.m_headers);
	this->m_payload = std::move(httpResponse.m_payload);

	return static_cast<HttpResponse&>(*this);
}

bool HttpResponse::operator== (const HttpResponse& httpResponse) const {

	if (this->m_statusCode != httpResponse.m_statusCode) return false;
	if (this->m_headers != httpResponse.m_headers) return false;
	if (this->m_payload != httpResponse.m_payload) return false;

	return true;
}

HttpStatusCode HttpResponse::GetStatusCode() const {
	return this->m_statusCode;
}

const HttpHeaders& HttpResponse::GetHeaders() const {
	return this->m_headers;
}

HttpHeaders& HttpResponse::GetHeaders() {
	return this->m_headers;
}

const std::vector<std::uint8_t>& HttpResponse::GetPayload() const {
	return this->m_payload;
}

std::vector<std::uint8_t>& HttpResponse::GetPayload() {
	return this->m_payload;
}

void HttpResponse::SetStatusCode(const std::uint32_t statusCode) {
	this->SetStatusCode(static_cast<HttpStatusCode>(statusCode));
}

void HttpResponse::SetStatusCode(const HttpStatusCode statusCode) {
	this->m_statusCode = statusCode;
}

void HttpResponse::SetHeaders(const HttpHeaders& httpHeaders) {
	this->m_headers = httpHeaders;
}

void HttpResponse::SetHeaders(HttpHeaders&& httpHeaders) noexcept {
	this->m_headers = std::move(httpHeaders);
}

void HttpResponse::SetPayload(const std::span<const std::uint8_t>& payload) {
	this->m_payload.resize(payload.size());
	memcpy_s(this->m_payload.data(), this->m_payload.size(), payload.data(), payload.size());
}

void HttpResponse::SetPayload(std::vector<std::uint8_t>&& payload) noexcept {
	this->m_payload = std::move(payload);
}

void HttpResponse::DeletePayload() {
	this->m_payload.clear();
}

void HttpResponse::Write(const std::string& text) {
	const std::size_t len = this->m_payload.size();
	this->m_payload.resize(len + text.size());
	memcpy_s((this->m_payload.data() + len), (this->m_payload.size() - len), text.c_str(), text.size());
}

HttpResponse HttpResponse::Parse(const std::span<const std::uint8_t>& data) {

	HttpResponse httpResponse;

	// convert the byte buffer into a string:
	std::string_view resstr = { reinterpret_cast<const char*>(data.data()), data.size() };

	// parse the first line of the response (version and status):
	std::size_t responseLineEnd = resstr.find("\r\n");
	if (responseLineEnd == std::string_view::npos)
		throw HttpException(HttpErrorType::RESPONSE_PARSING_ERROR, HttpErrorSubtype::GENERIC_ERROR);

	const std::size_t versionEnd = resstr.find(' ');
	if ((versionEnd == std::string_view::npos) || (versionEnd >= responseLineEnd))
		throw HttpException(HttpErrorType::RESPONSE_PARSING_ERROR, HttpErrorSubtype::GENERIC_ERROR);

	// check if the version is http/1.0 or http/1.1:
	const std::string_view versionStr = resstr.substr(0, versionEnd);
	if ((versionStr != "HTTP/1.0") && (versionStr != "HTTP/1.1"))
		throw HttpException(HttpErrorType::RESPONSE_PARSING_ERROR, HttpErrorSubtype::INVALID_HTTP_VERSION);

	resstr = resstr.substr(versionEnd + 1);
	responseLineEnd -= (strlen("HTTP/1.1"));

	const std::size_t statusCodeEnd = resstr.find(' ');
	if ((statusCodeEnd == std::string_view::npos) || (statusCodeEnd >= responseLineEnd))
		throw HttpException(HttpErrorType::RESPONSE_PARSING_ERROR, HttpErrorSubtype::GENERIC_ERROR);

	// check if the status code is valid:
	const std::string_view statusCodeStr = resstr.substr(0, statusCodeEnd);
	std::string_view::const_iterator it = std::find_if(statusCodeStr.begin(), statusCodeStr.end(), [&] (const char ch) -> bool {
		if ((ch >= '0') && (ch <= '9')) return false;
		else return true;
	});

	if (it != statusCodeStr.end())
		throw HttpException(HttpErrorType::RESPONSE_PARSING_ERROR, HttpErrorSubtype::INVALID_STATUS_CODE, "Non-numerical status code.");

	HttpStatusCode statusCode;
	try { statusCode = ToStatusCode(std::stoul(statusCodeStr.data())); }
	catch (const std::invalid_argument& ex) {
		throw HttpException(HttpErrorType::RESPONSE_PARSING_ERROR, HttpErrorSubtype::INVALID_STATUS_CODE, ex.what());
	}

	httpResponse.SetStatusCode(statusCode);
	resstr = resstr.substr(responseLineEnd + 1);

	if (resstr.empty()) return httpResponse;

	// parse http headers:
	bool headerParsing = (!resstr.substr(0, resstr.find("\r\n")).empty());
	std::vector<std::pair<std::string, std::string>> headers = { };
	while (headerParsing) {

		const std::string_view headerField = resstr.substr(0, resstr.find("\r\n"));

		const std::size_t cln = headerField.find(": ");
		if (cln == std::string_view::npos)
			throw HttpException(HttpErrorType::RESPONSE_PARSING_ERROR, HttpErrorSubtype::GENERIC_ERROR);

		const std::string headerName(headerField.substr(0, cln));
		const std::string headerValue(headerField.substr(cln + 2));
		headers.push_back({ headerName, headerValue });

		resstr = resstr.substr(resstr.find("\r\n") + 2);
		headerParsing = (!resstr.substr(0, resstr.find("\r\n")).empty());

	}

	std::sort(headers.begin(), headers.end());
	for (const auto& [name, value] : headers) {

		if (!HttpHeaders::IsValidHeaderName(name))
			throw HttpException(
				HttpErrorType::RESPONSE_PARSING_ERROR,
				HttpErrorSubtype::INVALID_HEADER_NAME,
				std::format(R"("{0}" is not a valid HTTP header name.)", name)
			);

		if (!HttpHeaders::IsValidHeaderValue(value))
			throw HttpException(
				HttpErrorType::RESPONSE_PARSING_ERROR,
				HttpErrorSubtype::INVALID_HEADER_VALUE,
				std::format(R"(Invalid character(s) in header "{0}".)", name)
			);

		httpResponse.GetHeaders().AddHeader(name, value);

	}

	resstr = resstr.substr(2);
	if (resstr.empty()) return httpResponse;

	// copy the payload:
	std::vector<std::uint8_t> payload(resstr.length());
	memcpy_s(payload.data(), payload.size(), resstr.data(), resstr.length());
	httpResponse.SetPayload(std::move(payload));

	return httpResponse;
}

std::vector<std::uint8_t> HttpResponse::Serialize(const HttpResponse& httpResponse) {

	std::ostringstream stream;

	// serialize the status code:
	std::string statusText;
	try { statusText = ToString(httpResponse.GetStatusCode()); }
	catch (const std::invalid_argument& ex) {
		throw HttpException(HttpErrorType::RESPONSE_SERIALIZATION_ERROR, HttpErrorSubtype::INVALID_STATUS_CODE, ex.what());
	}

	stream << "HTTP/1.1 ";
	stream << static_cast<int>(httpResponse.GetStatusCode()) << " " << statusText;
	stream << "\r\n";

	// serialize headers:
	std::vector<std::pair<std::string, std::string>> headers;
	headers = { httpResponse.GetHeaders().Begin(), httpResponse.GetHeaders().End() };
	std::sort(headers.begin(), headers.end());

	for (const auto& [name, value] : headers) {
		
		if (!HttpHeaders::IsValidHeaderName(name))
			throw HttpException(
				HttpErrorType::RESPONSE_SERIALIZATION_ERROR,
				HttpErrorSubtype::INVALID_HEADER_NAME,
				std::format(R"("{0}" is not a valid HTTP header name.)", name)
			);

		if (!HttpHeaders::IsValidHeaderValue(value))
			throw HttpException(
				HttpErrorType::RESPONSE_SERIALIZATION_ERROR,
				HttpErrorSubtype::INVALID_HEADER_VALUE,
				std::format(R"(Invalid character(s) in header "{0}".)", name)
			);

		stream << name << ": " << value << "\r\n";
		
	}

	stream << "\r\n";

	// serialize the payload:
	for (const std::uint8_t byte : httpResponse.GetPayload())
		stream << static_cast<char>(byte);

	const std::string str = { stream.str() };
	std::vector<std::uint8_t> data(str.length());
	memcpy_s(data.data(), data.size(), str.c_str(), str.length());

	return data;
}