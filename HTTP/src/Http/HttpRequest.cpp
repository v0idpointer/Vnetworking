#include <Vnetworking/Http/HttpRequest.h>

using namespace Vnetworking;
using namespace Vnetworking::Http;

HttpRequest::HttpRequest()
	: HttpRequest(HttpVersion::HTTP_1_1, HttpMethod::GET, Uri("http:/")) { }

HttpRequest::HttpRequest(const HttpMethod method, const Uri& requestUri)
	: HttpRequest(HttpVersion::HTTP_1_1, method, requestUri) { }

HttpRequest::HttpRequest(const HttpVersion version, const HttpMethod method, const Uri& requestUri)
	: m_version(version), m_method(method), m_requestUri(requestUri) { }

HttpRequest::HttpRequest(const HttpRequest& httpRequest) : m_requestUri(Uri("http:/")) {
	this->operator= (httpRequest);
}

HttpRequest::HttpRequest(HttpRequest&& httpRequest) noexcept : m_requestUri(Uri("http:/")) {
	this->operator= (std::move(httpRequest));
}

HttpRequest::~HttpRequest() { }

HttpRequest& HttpRequest::operator= (const HttpRequest& httpRequest) {

	this->m_version = httpRequest.m_version;
	this->m_method = httpRequest.m_method;
	this->m_requestUri = httpRequest.m_requestUri;
	this->m_payload = httpRequest.m_payload;

	return static_cast<HttpRequest&>(*this);
}

HttpRequest& HttpRequest::operator= (HttpRequest&& httpRequest) noexcept {

	this->m_version = std::move(httpRequest.m_version);
	this->m_method = std::move(httpRequest.m_method);
	this->m_requestUri = std::move(httpRequest.m_requestUri);
	this->m_payload = std::move(httpRequest.m_payload);

	return static_cast<HttpRequest&>(*this);
}

bool HttpRequest::operator== (const HttpRequest& httpRequest) const {

	if (this->m_version != httpRequest.m_version) return false;
	if (this->m_method != httpRequest.m_method) return false;
	if (this->m_requestUri != httpRequest.m_requestUri) return false;
	if (this->m_payload != httpRequest.m_payload) return false;

	return true;
}

HttpVersion HttpRequest::GetVersion() const {
	return this->m_version;
}

HttpMethod HttpRequest::GetMethod() const {
	return this->m_method;
}

const Uri& HttpRequest::GetRequestUri() const {
	return this->m_requestUri;
}

const std::vector<std::uint8_t>& HttpRequest::GetPayload() const {
	return this->m_payload;
}

std::vector<std::uint8_t>& HttpRequest::GetPayload() {
	return this->m_payload;
}

void HttpRequest::SetVersion(const HttpVersion version) {
	this->m_version = version;
}

void HttpRequest::SetMethod(const HttpMethod method) {
	this->m_method = method;
}

void HttpRequest::SetRequestUri(const Uri& requestUri) {
	this->m_requestUri = requestUri;
}

void HttpRequest::SetPayload(const std::vector<std::uint8_t>& payload) {
	this->m_payload = payload;
}

void HttpRequest::SetPayload(std::vector<std::uint8_t>&& payload) noexcept {
	this->m_payload = std::move(payload);
}

void HttpRequest::DeletePayload() {
	this->m_payload = { };
}