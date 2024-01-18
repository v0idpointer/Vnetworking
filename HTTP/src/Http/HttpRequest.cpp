#include <Vnetworking/Http/HttpRequest.h>

#include <sstream>
#include <algorithm>
#include <exception>
#include <stdexcept>

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
	this->m_headers = httpRequest.m_headers;
	this->m_payload = httpRequest.m_payload;

	return static_cast<HttpRequest&>(*this);
}

HttpRequest& HttpRequest::operator= (HttpRequest&& httpRequest) noexcept {

	this->m_version = std::move(httpRequest.m_version);
	this->m_method = std::move(httpRequest.m_method);
	this->m_requestUri = std::move(httpRequest.m_requestUri);
	this->m_headers = std::move(httpRequest.m_headers);
	this->m_payload = std::move(httpRequest.m_payload);

	return static_cast<HttpRequest&>(*this);
}

bool HttpRequest::operator== (const HttpRequest& httpRequest) const {

	if (this->m_version != httpRequest.m_version) return false;
	if (this->m_method != httpRequest.m_method) return false;
	if (this->m_requestUri != httpRequest.m_requestUri) return false;
	if (this->m_headers != httpRequest.m_headers) return false;
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

void HttpRequest::SetVersion(const HttpVersion version) {
	this->m_version = version;
}

void HttpRequest::SetMethod(const HttpMethod method) {
	this->m_method = method;
}

void HttpRequest::SetRequestUri(const Uri& requestUri) {
	this->m_requestUri = requestUri;
}

void HttpRequest::SetHeaders(const HttpHeaders& httpHeaders) {
	this->m_headers = httpHeaders;
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

HttpRequest HttpRequest::Parse(const std::vector<std::uint8_t>& data, const HttpVersion version) {

	if (!((version == HttpVersion::HTTP_1_0) || (version == HttpVersion::HTTP_1_1)))
		throw std::invalid_argument("Cannot parse HTTP request: unsupported HTTP version.");

	HttpRequest httpRequest;

	// convert the byte buffer into a string:
	std::string_view reqstr = { reinterpret_cast<const char*>(data.data()), data.size() };

	// parse the first line of the request (contains method, path and version):

	std::size_t requestLineEnd = reqstr.find("\r\n");
	if (requestLineEnd == std::string_view::npos)
		throw std::runtime_error("Bad HTTP request.");

	const std::size_t methodEnd = reqstr.find(' ');
	if ((methodEnd == std::string_view::npos) || (methodEnd >= requestLineEnd))
		throw std::runtime_error("Bad HTTP request.");

	// set http method. if the method is not valid, throw an exception:
	const std::string_view methodStr = reqstr.substr(0, methodEnd);
	const std::vector<HttpMethod> methods = {
		HttpMethod::GET, HttpMethod::HEAD, HttpMethod::POST, HttpMethod::PUT, HttpMethod::DELETE,
		HttpMethod::CONNECT, HttpMethod::OPTIONS, HttpMethod::TRACE, HttpMethod::PATCH, HttpMethod(0xFFFF),
	};

	for (const HttpMethod method : methods) {
		
		if (static_cast<std::uint16_t>(method) == 0xFFFF) 
			throw std::runtime_error("Bad HTTP request: invalid and/or unsupported HTTP method.");

		if (ToString(method) == methodStr) {
			httpRequest.SetMethod(method);
			break;
		}

	}

	reqstr = reqstr.substr(methodEnd + 1);
	requestLineEnd -= ToString(httpRequest.GetMethod()).length();

	// set the request uri:
	const std::size_t pathEnd = reqstr.find(' ');
	if ((pathEnd == std::string_view::npos) || (pathEnd >= requestLineEnd))
		throw std::runtime_error("Bad HTTP request.");

	std::string_view path = reqstr.substr(0, pathEnd);
	path = path.substr(path.find('/'));
	try {
		const Uri requestUri = { ("http:" + std::string(path)) };
		httpRequest.SetRequestUri(requestUri);
	}
	catch (const std::invalid_argument& ex) {
		std::ostringstream stream;
		stream << "Bad HTTP request: bad request URI: " << ex.what();
		throw std::runtime_error(stream.str());
	}
	catch (const std::length_error&) {
		throw std::runtime_error("Bad HTTP request: request URI too long.");
	}

	path = reqstr.substr(0, pathEnd);
	reqstr = reqstr.substr(pathEnd + 1);
	requestLineEnd -= (path.length() + 1);

	// check if the http version is 1.1 or 1.0:
	const std::size_t versionEnd = (requestLineEnd - 1);
	const std::string_view versionStr = reqstr.substr(0, versionEnd);
	if (!((versionStr == "HTTP/1.0") || (versionStr == "HTTP/1.1")))
		throw std::runtime_error("Bad HTTP request: unsupported HTTP version.");

	httpRequest.SetVersion((versionStr == "HTTP/1.1") ? HttpVersion::HTTP_1_1 : HttpVersion::HTTP_1_0);

	reqstr = reqstr.substr(versionEnd + 2);

	// parse http headers:
	
	// if the request string is empty, it means the request has no headers & no payload:
	if (reqstr.empty()) return httpRequest;
	
	// parse headers until the line containing CRLF is reached.
	bool headerParsing = ( !reqstr.substr(0, reqstr.find("\r\n")).empty() );
	while (headerParsing) {
		
		const std::string_view headerField = reqstr.substr(0, reqstr.find("\r\n"));
		
		const std::size_t cln = headerField.find(": ");
		if (cln == std::string_view::npos)
			throw std::runtime_error("bad http request");

		const std::string headerName(headerField.substr(0, cln));
		const std::string headerValue(headerField.substr(cln + 2));
		httpRequest.GetHeaders().AddHeader(headerName, headerValue);

		reqstr = reqstr.substr(reqstr.find("\r\n") + 2);
		headerParsing = (!reqstr.substr(0, reqstr.find("\r\n")).empty());
		
	}

	reqstr = reqstr.substr(2);

	// if the request string is empty, it means request has no payload data:
	if (reqstr.empty()) return httpRequest;

	// copy the payload:
	std::vector<std::uint8_t> payload(reqstr.length());
	memcpy_s(payload.data(), payload.size(), reqstr.data(), reqstr.length());
	httpRequest.SetPayload(std::move(payload));

	return httpRequest;
}

std::vector<std::uint8_t> HttpRequest::Serialize(const HttpRequest& httpRequest) {

	if (!((httpRequest.GetVersion() == HttpVersion::HTTP_1_0) || (httpRequest.GetVersion() == HttpVersion::HTTP_1_1)))
		throw std::invalid_argument("Cannot serialize HTTP request: unsupported HTTP version.");

	std::ostringstream stream;

	stream << ToString(httpRequest.GetMethod()) << " ";
	stream << httpRequest.GetRequestUri().GetPath().value();
	if (httpRequest.GetRequestUri().GetQuery().has_value())
		stream << "?" << httpRequest.GetRequestUri().GetQuery().value();
	stream << " " << ToString(httpRequest.GetVersion()) << "\r\n";

	std::vector<std::pair<std::string, std::string>> headers = { httpRequest.GetHeaders().Begin(), httpRequest.GetHeaders().End() };
	std::sort(headers.begin(), headers.end());
	for (const auto& [name, value] : headers)
		stream << name << ": " << value << "\r\n";

	stream << "\r\n";
	for (const std::uint8_t byte : httpRequest.GetPayload())
		stream << (char)(byte);

	const std::string str = (stream.str());
	std::vector<std::uint8_t> data(str.size());
	for (std::size_t i = 0; i < str.length(); ++i)
		data[i] = str[i];

	return data;
}