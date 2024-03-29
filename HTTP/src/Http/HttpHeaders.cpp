#include <Vnetworking/Http/HttpHeaders.h>

#include <algorithm>

using namespace Vnetworking::Http;

HttpHeaders::HttpHeaders() { }

HttpHeaders::HttpHeaders(const HttpHeaders& httpHeaders) {
	this->operator= (httpHeaders);
}

HttpHeaders::HttpHeaders(HttpHeaders&& httpHeaders) noexcept {
	this->operator= (std::move(httpHeaders));
}

HttpHeaders::~HttpHeaders() { }

HttpHeaders& HttpHeaders::operator= (const HttpHeaders& httpHeaders) {
	this->m_headers = { httpHeaders.m_headers.begin(), httpHeaders.m_headers.end() };
	return static_cast<HttpHeaders&>(*this);
}

HttpHeaders& HttpHeaders::operator= (HttpHeaders&& httpHeaders) noexcept {
	this->m_headers = std::move(httpHeaders.m_headers);
	httpHeaders.m_headers = { };
	return static_cast<HttpHeaders&>(*this);
}

bool HttpHeaders::operator== (const HttpHeaders& httpHeaders) const {
	return (this->m_headers == httpHeaders.m_headers);
}

std::list<std::pair<std::string, std::string>>::const_iterator HttpHeaders::Begin() const {
	return this->begin();
}

std::list<std::pair<std::string, std::string>>::const_iterator HttpHeaders::begin() const {
	return this->m_headers.begin();
}

std::list<std::pair<std::string, std::string>>::const_iterator HttpHeaders::End() const {
	return this->end();
}

std::list<std::pair<std::string, std::string>>::const_iterator HttpHeaders::end() const {
	return this->m_headers.end();
}

static void ToLowercase(std::string& str) noexcept {
	std::transform(str.begin(), str.end(), str.begin(), [&] (const char ch) -> char {
		return std::tolower(ch);
	});
}

std::optional<std::string> HttpHeaders::GetHeader(std::string headerName) const {

	ToLowercase(headerName);

	std::list<std::pair<std::string, std::string>>::const_iterator it;
	it = std::find_if(this->m_headers.begin(), this->m_headers.end(), [&] (const std::pair<std::string, std::string>& p) -> bool {
		return (p.first == headerName);
	});

	if (it == this->m_headers.end()) return std::nullopt;
	else return it->second;
}

std::vector<std::string> HttpHeaders::GetAllHeaders(std::string headerName) const {

	ToLowercase(headerName);

	std::vector<std::string> headerValues = { };
	std::for_each(this->m_headers.begin(), this->m_headers.end(), [&] (const std::pair<std::string, std::string>& p) -> void {
		if (p.first == headerName) headerValues.push_back(p.second);
	});

	return headerValues;
}

bool HttpHeaders::ContainsHeader(std::string headerName) const {

	ToLowercase(headerName);

	std::list<std::pair<std::string, std::string>>::const_iterator it;
	it = std::find_if(this->m_headers.begin(), this->m_headers.end(), [&] (const std::pair<std::string, std::string>& p) -> bool {
		return (p.first == headerName);
	});

	return (it != this->m_headers.end());
}

std::int32_t HttpHeaders::GetHeaderCount(std::string headerName) const {

	ToLowercase(headerName);

	std::int32_t count = 0;
	std::for_each(this->m_headers.begin(), this->m_headers.end(), [&] (const std::pair<std::string, std::string>& p) -> void {
		if (p.first == headerName) ++count;
	});

	return count;
}

std::int32_t HttpHeaders::GetHeaderCount() const {
	return static_cast<std::int32_t>(this->m_headers.size());
}

std::unordered_set<std::string> HttpHeaders::GetHeaderNames() const {

	std::unordered_set<std::string> names = { };
	std::for_each(this->m_headers.begin(), this->m_headers.end(), [&] (const std::pair<std::string, std::string>& p) -> void {
		names.insert(p.first);
	});

	return names;
}

void HttpHeaders::AddHeader(std::string headerName, const std::string& headerValue) {
	ToLowercase(headerName);
	this->m_headers.push_back({ headerName, headerValue });
}

void HttpHeaders::SetHeader(std::string headerName, const std::string& headerValue) {
	if (this->ContainsHeader(headerName))
		this->DeleteAllHeaders(headerName);
	this->AddHeader(headerName, headerValue);
}

bool HttpHeaders::DeleteHeader(std::string headerName) {

	ToLowercase(headerName);

	std::list<std::pair<std::string, std::string>>::const_iterator it;
	it = std::find_if(this->m_headers.begin(), this->m_headers.end(), [&] (const std::pair<std::string, std::string>& p) -> bool {
		return (p.first == headerName);
	});

	if (it == this->m_headers.end()) return false;

	this->m_headers.erase(it);
	return true;
}

std::int32_t HttpHeaders::DeleteAllHeaders(std::string headerName) {

	ToLowercase(headerName);

	unsigned long long removed = this->m_headers.remove_if([&] (const std::pair<std::string, std::string>& p) -> bool {
		return (p.first == headerName);
	});

	return static_cast<std::int32_t>(removed);
}

void HttpHeaders::Clear() {
	this->m_headers.clear();
}

bool HttpHeaders::IsValidHeaderName(std::string headerName) {

	ToLowercase(headerName);

	std::string::const_iterator it;
	it = std::find_if(headerName.begin(), headerName.end(), [&] (const char ch) -> bool {

		if ((ch >= 'A') && (ch <= 'Z')) return false;
		if ((ch >= 'a') && (ch <= 'z')) return false;
		if ((ch >= '0') && (ch <= '9')) return false;
		if ((ch == '-') || (ch == '_')) return false;

		return true;
	});

	return (it == headerName.end());
}

bool HttpHeaders::IsValidHeaderValue(const std::string& headerValue) {

	std::string::const_iterator it;
	const std::string allowedCharacters = R"(_ :;.,\/"'?!(){}[]@<>=-+*#$&`|~^%)";
	it = std::find_if(headerValue.begin(), headerValue.end(), [&](const char ch) -> bool {

		if ((ch >= 'A') && (ch <= 'Z')) return false;
		if ((ch >= 'a') && (ch <= 'z')) return false;
		if ((ch >= '0') && (ch <= '9')) return false;

		return (allowedCharacters.find(ch) == std::string::npos);
	});

	return (it == headerValue.end());
}