#include <Vnetworking/Http/HttpResponse.h>
#include <Vnetworking/Http/HttpException.h>

#include <sstream>
#include <algorithm>

using namespace Vnetworking::Http;

HttpResponse::HttpResponse()
	: HttpResponse(HttpVersion::HTTP_1_1, HttpStatusCode::NOT_FOUND) { }

HttpResponse::HttpResponse(const HttpStatusCode statusCode)
	: HttpResponse(HttpVersion::HTTP_1_1, statusCode) { }

HttpResponse::HttpResponse(const HttpVersion version, const HttpStatusCode statusCode)
	: m_version(version), m_statusCode(statusCode) { }

HttpResponse::HttpResponse(const HttpResponse& httpResponse) {
	this->operator= (httpResponse);
}

HttpResponse::HttpResponse(HttpResponse&& httpResponse) noexcept {
	this->operator= (std::move(httpResponse));
}

HttpResponse::~HttpResponse() { }

HttpResponse& HttpResponse::operator= (const HttpResponse& httpResponse) {

	this->m_version = httpResponse.m_version;
	this->m_statusCode = httpResponse.m_statusCode;
	this->m_headers = httpResponse.m_headers;
	this->m_payload = httpResponse.m_payload;

	return static_cast<HttpResponse&>(*this);
}

HttpResponse& HttpResponse::operator= (HttpResponse&& httpResponse) noexcept {

	this->m_version = std::move(httpResponse.m_version);
	this->m_statusCode = std::move(httpResponse.m_statusCode);
	this->m_headers = std::move(httpResponse.m_headers);
	this->m_payload = std::move(httpResponse.m_payload);

	return static_cast<HttpResponse&>(*this);
}

bool HttpResponse::operator== (const HttpResponse& httpResponse) const {

	if (this->m_version != httpResponse.m_version) return false;
	if (this->m_statusCode != httpResponse.m_statusCode) return false;
	if (this->m_headers != httpResponse.m_headers) return false;
	if (this->m_payload != httpResponse.m_payload) return false;

	return true;
}

HttpVersion HttpResponse::GetVersion() const {
	return this->m_version;
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

void HttpResponse::SetVersion(const HttpVersion version) {
	this->m_version = version;
}

void HttpResponse::SetStatusCode(const HttpStatusCode statusCode) {
	this->m_statusCode = statusCode;
}

void HttpResponse::SetHeaders(const HttpHeaders& httpHeaders) {
	this->m_headers = httpHeaders;
}

void HttpResponse::SetPayload(const std::vector<std::uint8_t>& payload) {
	this->m_payload = payload;
}

void HttpResponse::SetPayload(std::vector<std::uint8_t>&& payload) noexcept {
	this->m_payload = std::move(payload);
}

void HttpResponse::DeletePayload() {
	this->m_payload = { };
}

std::vector<std::uint8_t> HttpResponse::Serialize(const HttpResponse& httpResponse) {

	if (!((httpResponse.GetVersion() == HttpVersion::HTTP_1_0) || (httpResponse.GetVersion() == HttpVersion::HTTP_1_1)))
		throw HttpException(HttpStatusCode::HTTP_VERSION_NOT_SUPPORTED);

	std::ostringstream stream;

	stream << ToString(httpResponse.GetVersion()) << " ";
	stream << static_cast<std::uint32_t>(httpResponse.GetStatusCode()) << " ";
	stream << ToString(httpResponse.GetStatusCode()) << "\r\n";

	std::vector<std::pair<std::string, std::string>> headers = { httpResponse.GetHeaders().Begin(), httpResponse.GetHeaders().End() };
	std::sort(headers.begin(), headers.end());
	for (const auto& [name, value] : headers)
		stream << name << ": " << value << "\r\n";

	stream << "\r\n";
	for (const std::uint8_t byte : httpResponse.GetPayload())
		stream << (char)(byte);

	const std::string str = (stream.str());
	std::vector<std::uint8_t> data(str.size());
	for (std::size_t i = 0; i < str.length(); ++i)
		data[i] = str[i];

	return data;
}