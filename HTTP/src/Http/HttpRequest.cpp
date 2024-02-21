#include <Vnetworking/Http/HttpRequest.h>
#include <Vnetworking/Http/HttpException.h>
#include <Vnetworking/BadUriException.h>

#include <sstream>
#include <format>
#include <algorithm>

using namespace Vnetworking;
using namespace Vnetworking::Http;

HttpRequest::HttpRequest()
	: HttpRequest(HttpMethod::GET, "/") { }

HttpRequest::HttpRequest(const HttpMethod method, const Uri& uri)
	: m_requestUri(uri), m_method(method), m_headers({ }), m_payload({ }) { }

HttpRequest::HttpRequest(const HttpMethod method, const std::string& uri)
	: HttpRequest(method, Uri(uri)) { }

HttpRequest::HttpRequest(const HttpRequest& httpRequest) : m_requestUri("") {
	this->operator= (httpRequest);
}

HttpRequest::HttpRequest(HttpRequest&& httpRequest) noexcept : m_requestUri("") {
	this->operator= (std::move(httpRequest));
}

HttpRequest::~HttpRequest() { }

HttpRequest& HttpRequest::operator= (const HttpRequest& httpRequest) {

	this->m_requestUri = httpRequest.m_requestUri;
	this->m_method = httpRequest.m_method;
	this->m_headers = httpRequest.m_headers;
	this->m_payload = httpRequest.m_payload;

	return static_cast<HttpRequest&>(*this);
}

HttpRequest& HttpRequest::operator= (HttpRequest&& httpRequest) noexcept {

	this->m_requestUri = std::move(httpRequest.m_requestUri);
	this->m_method = httpRequest.m_method;
	this->m_headers = std::move(httpRequest.m_headers);
	this->m_payload = std::move(httpRequest.m_payload);

	return static_cast<HttpRequest&>(*this);
}

bool HttpRequest::operator== (const HttpRequest& httpRequest) const {

	if (this->m_requestUri != httpRequest.m_requestUri) return false;
	if (this->m_method != httpRequest.m_method) return false;
	if (this->m_headers != httpRequest.m_headers) return false;
	if (this->m_payload != httpRequest.m_payload) return false;

	return true;
}

const Uri& HttpRequest::GetRequestUri() const {
	return this->m_requestUri;
}

HttpMethod HttpRequest::GetMethod() const {
	return this->m_method;
}

const HttpHeaders& HttpRequest::GetHeaders() const {
	return this->m_headers;
}

HttpHeaders& HttpRequest::GetHeaders() {
	return this->m_headers;
}

const std::vector<std::uint8_t>& HttpRequest::GetPayload() const {
	return this->m_payload;
}

std::vector<std::uint8_t>& HttpRequest::GetPayload() {
	return this->m_payload;
}

void HttpRequest::SetRequestUri(const Uri& uri) {
	this->m_requestUri = uri;
}

void HttpRequest::SetRequestUri(Uri&& uri) noexcept {
	this->m_requestUri = std::move(uri);
}

void HttpRequest::SetRequestUri(const std::string& uri) {
	this->m_requestUri = Uri(uri);
}

void HttpRequest::SetMethod(const HttpMethod method) {
	this->m_method = method;
}

void HttpRequest::SetHeaders(const HttpHeaders& httpHeaders) {
	this->m_headers = httpHeaders;
}

void HttpRequest::SetHeaders(HttpHeaders&& httpHeaders) noexcept {
	this->m_headers = std::move(httpHeaders);
}

void HttpRequest::SetPayload(const std::span<const std::uint8_t>& payload) {
	this->m_payload.resize(payload.size());
	memcpy_s(this->m_payload.data(), this->m_payload.size(), payload.data(), payload.size());
}

void HttpRequest::SetPayload(std::vector<std::uint8_t>&& payload) noexcept {
	this->m_payload = std::move(payload);
}

void HttpRequest::DeletePayload() {
	this->m_payload.clear();
}

void HttpRequest::Write(const std::string& text) {
	const std::size_t len = this->m_payload.size();
	this->m_payload.resize(len + text.size());
	memcpy_s((this->m_payload.data() + len), (this->m_payload.size() - len), text.c_str(), text.size());
}

HttpRequest HttpRequest::Parse(const std::span<const std::uint8_t>& data) {

	HttpRequest httpRequest;

	// convert the byte buffer into a string:
	std::string_view reqstr = { reinterpret_cast<const char*>(data.data()), data.size() };

	// parse the first line of the request (method, uri and version):
	std::size_t requestLineEnd = reqstr.find("\r\n");
	if (requestLineEnd == std::string_view::npos)
		throw HttpException(HttpErrorType::REQUEST_PARSING_ERROR, HttpErrorSubtype::GENERIC_ERROR);

	const std::size_t methodEnd = reqstr.find(' ');
	if ((methodEnd == std::string_view::npos) || (methodEnd >= requestLineEnd))
		throw HttpException(HttpErrorType::REQUEST_PARSING_ERROR, HttpErrorSubtype::GENERIC_ERROR);

	// set the method:
	HttpMethod method = static_cast<HttpMethod>(0);
	const std::string_view methodStr = reqstr.substr(0, methodEnd);
	try { method = ToMethod(std::string(methodStr)); }
	catch (const std::invalid_argument& ex) {
		throw HttpException(HttpErrorType::REQUEST_PARSING_ERROR, HttpErrorSubtype::INVALID_METHOD, ex.what());
	}

	httpRequest.SetMethod(method);
	reqstr = reqstr.substr(methodEnd + 1);
	requestLineEnd -= ToString(method).length();

	// set the request uri:
	const std::size_t pathEnd = reqstr.find(' ');
	if ((pathEnd == std::string_view::npos) || (pathEnd >= requestLineEnd))
		throw HttpException(HttpErrorType::REQUEST_PARSING_ERROR, HttpErrorSubtype::GENERIC_ERROR);

	std::string_view path = reqstr.substr(0, pathEnd);
	path = path.substr(path.find('/'));
	Uri uri = { "/" };
	try { uri = Uri(std::string(path)); }
	catch (const BadUriException& ex) {
		throw HttpException(HttpErrorType::REQUEST_PARSING_ERROR, HttpErrorSubtype::INVALID_REQUEST_URI, ex.what());
	}

	path = reqstr.substr(0, pathEnd);
	reqstr = reqstr.substr(pathEnd + 1);
	requestLineEnd -= (path.length() + 1);

	// check if the version is http/1.0 or http/1.1:
	const std::size_t versionEnd = (requestLineEnd - 1);
	const std::string_view versionStr = reqstr.substr(0, versionEnd);
	if ((versionStr != "HTTP/1.0") && (versionStr != "HTTP/1.1"))
		throw HttpException(HttpErrorType::REQUEST_PARSING_ERROR, HttpErrorSubtype::INVALID_HTTP_VERSION);

	reqstr = reqstr.substr(versionEnd + 2);
	if (reqstr.empty()) return httpRequest;

	// parse http headers:
	bool headerParsing = (!reqstr.substr(0, reqstr.find("\r\n")).empty());
	std::vector<std::pair<std::string, std::string>> headers = { };
	while (headerParsing) {

		const std::string_view headerField = reqstr.substr(0, reqstr.find("\r\n"));

		const std::size_t cln = headerField.find(": ");
		if (cln == std::string_view::npos)
			throw HttpException(HttpErrorType::REQUEST_PARSING_ERROR, HttpErrorSubtype::GENERIC_ERROR);

		const std::string headerName(headerField.substr(0, cln));
		const std::string headerValue(headerField.substr(cln + 2));
		headers.push_back({ headerName, headerValue });

		reqstr = reqstr.substr(reqstr.find("\r\n") + 2);
		headerParsing = (!reqstr.substr(0, reqstr.find("\r\n")).empty());

	}

	std::sort(headers.begin(), headers.end());
	for (const auto& [name, value] : headers) {

		if (!HttpHeaders::IsValidHeaderName(name))
			throw HttpException(
				HttpErrorType::REQUEST_PARSING_ERROR,
				HttpErrorSubtype::INVALID_HEADER_NAME,
				std::format(R"("{0}" is not a valid HTTP header name.)", name)
			);

		if (!HttpHeaders::IsValidHeaderValue(value))
			throw HttpException(
				HttpErrorType::REQUEST_PARSING_ERROR,
				HttpErrorSubtype::INVALID_HEADER_VALUE,
				std::format(R"(Invalid character(s) in header "{0}".)", name)
			);

		httpRequest.GetHeaders().AddHeader(name, value);

	}

	reqstr = reqstr.substr(2);
	if (reqstr.empty()) return httpRequest;

	// copy the payload:
	std::vector<std::uint8_t> payload(reqstr.length());
	memcpy_s(payload.data(), payload.size(), reqstr.data(), reqstr.length());
	httpRequest.SetPayload(std::move(payload));

	return httpRequest;
}

std::vector<std::uint8_t> HttpRequest::Serialize(const HttpRequest& httpRequest) {

	std::ostringstream stream;

	// serialize method and uri:
	std::string methodText;
	try { methodText = ToString(httpRequest.GetMethod()); }
	catch (const std::invalid_argument& ex) {
		throw HttpException(HttpErrorType::REQUEST_SERIALIZATION_ERROR, HttpErrorSubtype::INVALID_METHOD, ex.what());
	}

	stream << methodText << " ";
	stream << httpRequest.GetRequestUri().ToString() << " ";
	stream << "HTTP/1.1\r\n";

	// serialize headers:
	std::vector<std::pair<std::string, std::string>> headers;
	headers = { httpRequest.GetHeaders().Begin(), httpRequest.GetHeaders().End() };
	std::sort(headers.begin(), headers.end());

	for (const auto& [name, value] : headers) {

		if (!HttpHeaders::IsValidHeaderName(name))
			throw HttpException(
				HttpErrorType::REQUEST_SERIALIZATION_ERROR,
				HttpErrorSubtype::INVALID_HEADER_NAME,
				std::format(R"("{0}" is not a valid HTTP header name.)", name)
			);

		if (!HttpHeaders::IsValidHeaderValue(value))
			throw HttpException(
				HttpErrorType::REQUEST_SERIALIZATION_ERROR,
				HttpErrorSubtype::INVALID_HEADER_VALUE,
				std::format(R"(Invalid character(s) in header "{0}".)", name)
			);

		stream << name << ": " << value << "\r\n";

	}

	stream << "\r\n";

	// serialize the payload:
	for (const std::uint8_t byte : httpRequest.GetPayload())
		stream << static_cast<char>(byte);

	const std::string str = { stream.str() };
	std::vector<std::uint8_t> data(str.length());
	memcpy_s(data.data(), data.size(), str.c_str(), str.length());

	return data;
}