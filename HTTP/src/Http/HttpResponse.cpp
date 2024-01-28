#include <Vnetworking/Http/HttpResponse.h>
#include <Vnetworking/Http/HttpException.h>

#include <format>
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

// parse function for HTTP/0.9 responses
static HttpResponse Parse_0_9(const std::vector<std::uint8_t>& data) {

	HttpResponse httpResponse = { HttpVersion::HTTP_0_9, HttpStatusCode::OK };
	httpResponse.SetPayload(data);

	return httpResponse;
}

// parse function for HTTP/1.0 and HTTP/1.1 responses
static HttpResponse Parse_1_x(const std::vector<std::uint8_t>& data, const HttpVersion version) {

	HttpResponse httpResponse;

	// convert the byte buffer into a string:
	std::string_view resstr = { reinterpret_cast<const char*>(data.data()), data.size() };

	// parse the first line of the response (version and status):

	std::size_t responseLineEnd = resstr.find("\r\n");
	if (responseLineEnd == std::string_view::npos)
		throw HttpException(HttpStatusCode::BAD_REQUEST, "Bad response.");

	const std::size_t versionEnd = resstr.find(' ');
	if ((versionEnd == std::string_view::npos) || (versionEnd >= responseLineEnd))
		throw HttpException(HttpStatusCode::BAD_REQUEST, "Bad response.");

	// check if the version is http 1.0 or 1.1:
	const std::string_view versionStr = resstr.substr(0, versionEnd);
	if (versionStr != ToString(version))
		throw HttpException(HttpStatusCode::HTTP_VERSION_NOT_SUPPORTED);

	httpResponse.SetVersion(version);

	resstr = resstr.substr(versionEnd + 1);
	responseLineEnd -= ToString(httpResponse.GetVersion()).length();

	const std::size_t statusCodeEnd = resstr.find(' ');
	if ((statusCodeEnd == std::string_view::npos) || (statusCodeEnd >= responseLineEnd))
		throw HttpException(HttpStatusCode::BAD_REQUEST, "Bad response.");

	// check if the status code is valid:
	const std::string_view statusCodeStr = resstr.substr(0, statusCodeEnd);
	std::string_view::const_iterator it = std::find_if(statusCodeStr.begin(), statusCodeStr.end(), [&] (const char ch) -> bool {
		if ((ch >= '0') && (ch <= '9')) return false;
		else return true;
	});

	if (it != statusCodeStr.end())
		throw HttpException(HttpStatusCode::BAD_REQUEST, "Bad HTTP status code.");

	// set the status code:
	const HttpStatusCode statusCode = HttpStatusCode(std::stoul(statusCodeStr.data()));

	const bool validStatusCode = [&] (void) -> bool {

		// TODO: if version is HTTP/1.0, check if the status code is a valid 1.0 status code.

		try { const auto str = ToString(statusCode); }
		catch (const std::exception&) {
			return false;
		}

		return true;
	}();

	if (!validStatusCode)
		throw HttpException(HttpStatusCode::BAD_REQUEST, "Bad HTTP status code.");

	httpResponse.SetStatusCode(statusCode);
	resstr = resstr.substr(responseLineEnd + 1);

	// parse http headers:

	if (resstr.empty()) return httpResponse;

	// parse headers until the line containing CRLF is reached.
	bool headerParsing = (!resstr.substr(0, resstr.find("\r\n")).empty());
	std::vector<std::pair<std::string, std::string>> headers = { };
	while (headerParsing) {

		const std::string_view headerField = resstr.substr(0, resstr.find("\r\n"));

		const std::size_t cln = headerField.find(": ");
		if (cln == std::string_view::npos)
			throw HttpException(HttpStatusCode::BAD_REQUEST, "Bad response.");

		const std::string headerName(headerField.substr(0, cln));
		const std::string headerValue(headerField.substr(cln + 2));
		headers.push_back({ headerName, headerValue });

		resstr = resstr.substr(resstr.find("\r\n") + 2);
		headerParsing = (!resstr.substr(0, resstr.find("\r\n")).empty());

	}

	std::sort(headers.begin(), headers.end());
	for (const auto& [headerName, headerValue] : headers) {

		try { httpResponse.GetHeaders().AddHeader(headerName, headerValue); }
		catch (const std::invalid_argument& ex) {
			throw HttpException(HttpStatusCode::BAD_REQUEST, std::format("Bad response header(s): {0}", ex.what()));
		}

	}

	resstr = resstr.substr(2);

	if (resstr.empty()) return httpResponse;

	// copy the payload:
	std::vector<std::uint8_t> payload(resstr.length());
	memcpy_s(payload.data(), payload.size(), resstr.data(), resstr.length());
	httpResponse.SetPayload(std::move(payload));

	return httpResponse;
}

HttpResponse HttpResponse::Parse(const std::vector<std::uint8_t>& data, const HttpVersion version) {

	switch (version) {

	case HttpVersion::HTTP_0_9:
		return Parse_0_9(data);
		break;

	case HttpVersion::HTTP_1_0:
	case HttpVersion::HTTP_1_1:
		return Parse_1_x(data, version);
		break;

	default:
		throw HttpException(HttpStatusCode::HTTP_VERSION_NOT_SUPPORTED);
		break;

	}

}

static std::vector<std::uint8_t> Serialize_0_9(const HttpResponse& httpResponse) { 
	
	if (httpResponse.GetStatusCode() != HttpStatusCode::OK)
		throw HttpException("HTTP status codes are not supported in this HTTP version.");

	if (httpResponse.GetHeaders().GetHeaderCount() > 0)
		throw HttpException("HTTP headers are not supported in this HTTP version.");
	
	return { httpResponse.GetPayload().begin(), httpResponse.GetPayload().end() };
}

static std::vector<std::uint8_t> Serialize_1_x(const HttpResponse& httpResponse) { 

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

std::vector<std::uint8_t> HttpResponse::Serialize(const HttpResponse& httpResponse) {

	switch (httpResponse.GetVersion()) {

	case HttpVersion::HTTP_0_9:
		return Serialize_0_9(httpResponse);
		break;

	case HttpVersion::HTTP_1_0:
	case HttpVersion::HTTP_1_1:
		return Serialize_1_x(httpResponse);
		break;

	default:
		throw HttpException(HttpStatusCode::HTTP_VERSION_NOT_SUPPORTED);
		break;

	}

}