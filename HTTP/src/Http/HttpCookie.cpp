#include <Vnetworking/Http/HttpCookie.h>

#include <sstream>

using namespace Vnetworking;
using namespace Vnetworking::Http;

HttpCookie::HttpCookie() : HttpCookie("", "") { }

HttpCookie::HttpCookie(const std::string& name, const std::string& value) {

	this->m_name = name;
	this->m_value = value;
	this->m_expiration = std::nullopt;
	this->m_maxAge = std::nullopt;
	this->m_domain = std::nullopt;
	this->m_path = std::nullopt;
	this->m_secure = false;
	this->m_httpOnly = false;
	this->m_sameSite = std::nullopt;

}

HttpCookie::HttpCookie(const HttpCookie& httpCookie) {
	this->operator= (httpCookie);
}

HttpCookie::HttpCookie(HttpCookie&& httpCookie) noexcept {
	this->operator= (std::move(httpCookie));
}

HttpCookie::~HttpCookie() { }

HttpCookie& HttpCookie::operator= (const HttpCookie& httpCookie) {

	this->m_name = httpCookie.m_name;
	this->m_value = httpCookie.m_value;
	this->m_expiration = httpCookie.m_expiration;
	this->m_maxAge = httpCookie.m_maxAge;
	this->m_domain = httpCookie.m_domain;
	this->m_path = httpCookie.m_path;
	this->m_secure = httpCookie.m_secure;
	this->m_httpOnly = httpCookie.m_httpOnly;
	this->m_sameSite = httpCookie.m_sameSite;

	return static_cast<HttpCookie&>(*this);
}

HttpCookie& HttpCookie::operator= (HttpCookie&& httpCookie) noexcept {

	this->m_name = std::move(httpCookie.m_name);
	this->m_value = std::move(httpCookie.m_value);
	this->m_expiration = std::move(httpCookie.m_expiration);
	this->m_maxAge = httpCookie.m_maxAge;
	this->m_domain = std::move(httpCookie.m_domain);
	this->m_path = std::move(httpCookie.m_path);
	this->m_secure = httpCookie.m_secure;
	this->m_httpOnly = httpCookie.m_httpOnly;
	this->m_sameSite = std::move(httpCookie.m_sameSite);

	return static_cast<HttpCookie&>(*this);
}

bool HttpCookie::operator== (const HttpCookie& httpCookie) const {

	if (this->m_name != httpCookie.m_name) return false;
	if (this->m_value != httpCookie.m_value) return false;
	if (this->m_expiration != httpCookie.m_expiration) return false;
	if (this->m_maxAge != httpCookie.m_maxAge) return false;
	if (this->m_domain != httpCookie.m_domain) return false;
	if (this->m_path != httpCookie.m_path) return false;
	if (this->m_secure != httpCookie.m_secure) return false;
	if (this->m_httpOnly != httpCookie.m_httpOnly) return false;
	if (this->m_sameSite != httpCookie.m_sameSite) return false;

	return true;
}

std::string HttpCookie::GetName() const {
	return this->m_name;
}

std::string HttpCookie::GetValue() const {
	return this->m_value;
}

std::optional<Date> HttpCookie::GetExpirationDate() const {
	return this->m_expiration;
}

std::optional<std::int32_t> HttpCookie::GetMaxAge() const {
	return this->m_maxAge;
}

std::optional<std::string> HttpCookie::GetDomain() const {
	return this->m_domain;
}

std::optional<std::string> HttpCookie::GetPath() const {
	return this->m_path;
}

bool HttpCookie::IsSecure() const {
	return this->m_secure;
}

bool HttpCookie::IsHttpOnly() const {
	return this->m_httpOnly;
}

std::optional<std::string> HttpCookie::GetSameSite() const {
	return this->m_sameSite;
}

void HttpCookie::SetName(const std::string& name) {
	this->m_name = name;
}

void HttpCookie::SetValue(const std::string& value) {
	this->m_value = value;
}

void HttpCookie::SetExpirationDate(const std::optional<Date>& expirationDate) {
	this->m_expiration = expirationDate;
}

void HttpCookie::SetMaxAge(const std::optional<std::int32_t> maxAge) {
	this->m_maxAge = maxAge;
}

void HttpCookie::SetDomain(const std::optional<std::string>& domain) {
	this->m_domain = domain;
}

void HttpCookie::SetPath(const std::optional<std::string>& path) {
	this->m_path = path;
}

void HttpCookie::SetSecure(const bool secure) {
	this->m_secure = secure;
}

void HttpCookie::SetHttpOnly(const bool httpOnly) {
	this->m_httpOnly = httpOnly;
}

void HttpCookie::SetSameSite(const std::optional<std::string>& sameSite) {
	this->m_sameSite = sameSite;
}

std::string HttpCookie::ToString() const {
	
	std::ostringstream stream;

	stream << this->m_name << "=" << this->m_value;
	
	if (this->m_expiration.has_value())
		stream << "; Expires=" << this->m_expiration->ToUTCString();

	if (this->m_maxAge)
		stream << "; Max-Age=" << this->m_maxAge.value();

	if (this->m_domain.has_value())
		stream << "; Domain=" << this->m_domain.value();

	if (this->m_path.has_value())
		stream << "; Path=" << this->m_path.value();

	if (this->m_sameSite.has_value())
		stream << "; SameSite=" << this->m_sameSite.value();

	if (this->m_secure)
		stream << "; Secure";

	if (this->m_httpOnly)
		stream << "; HttpOnly";

	return stream.str();
}