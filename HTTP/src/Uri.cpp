#include <Vnetworking/Uri.h>

#include <algorithm>
#include <exception>
#include <stdexcept>

using namespace Vnetworking;

Uri::Uri(const std::string& uriString) {
	
	if (uriString.length() >= 32768)
		throw std::runtime_error("URI too long.");

	// check if the uri contains invalid character(s):
	std::string::const_iterator it = std::find_if(uriString.begin(), uriString.end(), [&] (const char ch) -> bool {
		
		// check for unreserved characters:
		if (((ch >= 'A') && (ch <= 'Z')) || ((ch >= 'a') && (ch <= 'z')) || ((ch >= '0') && (ch <= '9'))) return false;
		if ((ch == '-') || (ch == '_') || (ch == '.') || (ch == '~')) return false;

		// check for reserved characters:
		if ((ch == ':') || (ch == '/') || (ch == '?') || (ch == '#') || (ch == '[') || (ch == ']') || (ch == '@')) return false;
		if ((ch == '!') || (ch == '$') || (ch == '&') || (ch == '\'') || (ch == '(') || (ch == ')')) return false;
		if ((ch == '*') || (ch == '+') || (ch == ',') || (ch == ';') || (ch == '=')) return false;

		if (ch == '%') return false;

		return true;
	});
	
	if (it != uriString.end())
		throw std::runtime_error("URI contains invalid character(s).");

	// parse the scheme:
	if (uriString.find(':') == std::string::npos)
		throw std::runtime_error("Bad URI.");

	std::string scheme = uriString.substr(0, uriString.find(':'));
	it = std::find_if(scheme.begin(), scheme.end(), [&] (const char ch) -> bool {
		if (std::isalnum(ch) || (ch == '+') || (ch == '.') || (ch == '-')) return false;
		return true;
	});

	if (scheme.empty()) throw std::runtime_error("Bad URI: scheme is an empty string.");
	if (it != scheme.end()) throw std::runtime_error("Bad URI: scheme contains invalid character(s).");
	this->m_scheme = scheme;

	// parse the user info, host, port and path:
	std::string path = uriString.substr(uriString.find(':') + 1);
	if (path.starts_with("//")) {

		std::string authority = path.substr(2);
		if (authority.find('/') != std::string::npos) 
			authority = authority.substr(0, authority.find('/'));
		path = path.substr(authority.length() + 2);
	
		std::optional<std::string> userInfo;
		if (authority.find('@') != std::string::npos) {
			userInfo = authority.substr(0, authority.find('@'));
			authority = authority.substr(authority.find('@') + 1);
		}
		this->m_userInfo = userInfo;

		std::optional<std::string> port;
		if (authority.find(':') != std::string::npos) {
			port = authority.substr(authority.find(':') + 1);
			authority = authority.substr(0, authority.find(':'));
		}

		if (authority.empty()) throw std::runtime_error("Bad URI: host is an empty string.");
		this->m_host = authority;

		if (port) {

			it = std::find_if(port.value().begin(), port.value().end(), [&] (const char ch) -> bool {
				return (!std::isdigit(ch));
			});
			if (it != port.value().end()) throw std::runtime_error("Bad URI: non-numerical port.");

			std::uint16_t portNum = static_cast<std::uint16_t>(std::strtoul(port.value().c_str(), nullptr, 0));
			this->m_port = portNum;

		}

	}

	std::string tmp = path; // store the original path (with query string and/or fragment) for later
	if (path.empty()) path = "/";
	if (path.find('?') != std::string::npos) path = path.substr(0, path.find('?'));
	if (path.find('#') != std::string::npos) path = path.substr(0, path.find('#'));
	while (path.starts_with("//")) path = path.substr(1);

	this->m_path = path;

	// parse the query string and fragment:
	if (tmp.find('?') != std::string::npos) {
		std::string query = tmp.substr(tmp.find('?') + 1);
		query = query.substr(0, query.find('#'));
		if (!query.empty()) this->m_query = query;
	}

	if (tmp.find('#') != std::string::npos) {
		std::string fragment = tmp.substr(tmp.find('#') + 1);
		if (!fragment.empty()) this->m_fragment = fragment;
	}

	this->m_uriString = uriString;

}

Uri::Uri(const Uri& uri) {
	this->operator= (uri);
}

Uri::Uri(Uri&& uri) noexcept {
	this->operator= (std::move(uri));
}

Uri::~Uri() { }

Uri& Uri::operator= (const Uri& uri) {
	
	this->m_uriString = uri.m_uriString;
	this->m_scheme = uri.m_scheme;
	this->m_userInfo = uri.m_userInfo;
	this->m_host = uri.m_host;
	this->m_port = uri.m_port;
	this->m_path = uri.m_path;
	this->m_query = uri.m_query;
	this->m_fragment = uri.m_fragment;

	return static_cast<Uri&>(*this);
}

Uri& Uri::operator= (Uri&& uri) noexcept {
	
	this->m_uriString = std::move(uri.m_uriString);
	this->m_scheme = std::move(uri.m_scheme);
	this->m_userInfo = std::move(uri.m_userInfo);
	this->m_host = std::move(uri.m_host);
	this->m_port = std::move(uri.m_port);
	this->m_path = std::move(uri.m_path);
	this->m_query = std::move(uri.m_query);
	this->m_fragment = std::move(uri.m_fragment);

	return static_cast<Uri&>(*this);
}

bool Uri::operator== (const Uri& uri) const {
	return (this->m_uriString == uri.m_uriString);
}

std::optional<std::string> Uri::GetScheme() const {
	return this->m_scheme;
}

std::optional<std::string> Uri::GetUserInfo() const {
	return this->m_userInfo;
}

std::optional<std::string> Uri::GetHost() const {
	return this->m_host;
}

std::optional<std::uint16_t> Uri::GetPort() const {
	return this->m_port;
}

std::optional<std::string> Uri::GetPath() const {
	return this->m_path;
}

std::optional<std::string> Uri::GetQuery() const {
	return this->m_query;
}

std::optional<std::string> Uri::GetFragment() const {
	return this->m_fragment;
}