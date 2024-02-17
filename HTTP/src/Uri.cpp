#include <Vnetworking/Uri.h>
#include <Vnetworking/BadUriException.h>

#include <algorithm>
#include <sstream>

using namespace Vnetworking;

constexpr std::string_view ERR_URI_TOO_LONG = "URI too long.";
constexpr std::string_view ERR_INVALID_CHARACTERS = "URI contains invalid character(s).";
constexpr std::string_view ERR_SCHEME_MISSING = "URI scheme missing.";
constexpr std::string_view ERR_EMPTY_SCHEME = "URI scheme cannot be an empty string.";
constexpr std::string_view ERR_INVALID_CHAR_IN_SCHEME = "URI scheme contains invalid character(s).";
constexpr std::string_view ERR_BAD_HOST_FMT = "Bad URI host format.";
constexpr std::string_view ERR_URI_HOST_EMPTY = "URI host cannot be an empty string.";
constexpr std::string_view ERR_NON_NUMERICAL_PORT = "URI port cannot be a non-numerical value.";
constexpr std::string_view ERR_PATH_COMPONENT_MISSING = "URI path component missing.";

Uri::Uri(const std::string& uriString) {
	
	if (uriString.length() >= 32768)
		throw BadUriException(ERR_URI_TOO_LONG.data());

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
		throw BadUriException(ERR_INVALID_CHARACTERS.data());

	// if the uri string starts with a forward slash, parse it as a relative uri.
	// this is pretty much copy-paste path query and fragment parser.
	const bool isRelativeUri = (uriString.find('/') == 0);
	if (isRelativeUri) {
		
		std::string path = uriString;
		std::string tmp = path;
		if (path.find('?') != std::string::npos) path = path.substr(0, path.find('?'));
		if (path.find('#') != std::string::npos) path = path.substr(0, path.find('#'));
		while (path.starts_with("//")) path = path.substr(1);
		while (path.ends_with('/')) path = path.substr(0, (path.length() - 1));

		std::size_t pos = 0;
		while ((pos = path.find("//", pos)) != std::string::npos)
			path.replace(pos, 2, "/");

		if (path.empty()) path = "/";
		else if (path[0] != '/') path = ("/" + path);

		this->m_path = path;

		if (tmp.find('?') != std::string::npos) {
			std::string query = tmp.substr(tmp.find('?') + 1);
			query = query.substr(0, query.find('#'));
			if (!query.empty()) this->m_query = query;
		}

		if (tmp.find('#') != std::string::npos) {
			std::string fragment = tmp.substr(tmp.find('#') + 1);
			if (!fragment.empty()) this->m_fragment = fragment;
		}

		return;
	}

	// parse the scheme:
	if (uriString.find(':') == std::string::npos)
		throw BadUriException(ERR_SCHEME_MISSING.data());

	std::string scheme = uriString.substr(0, uriString.find(':'));
	it = std::find_if(scheme.begin(), scheme.end(), [&] (const char ch) -> bool {
		if (std::isalnum(ch) || (ch == '+') || (ch == '.') || (ch == '-')) return false;
		return true;
	});

	if (scheme.empty()) throw BadUriException(ERR_EMPTY_SCHEME.data());
	if (it != scheme.end()) throw BadUriException(ERR_INVALID_CHAR_IN_SCHEME.data());
	std::transform(scheme.begin(), scheme.end(), scheme.begin(), [&] (const char ch) -> char {
		return std::tolower(ch);
	});

	this->m_scheme = scheme;

	// parse the user info, host, port and path:
	std::string path = uriString.substr(uriString.find(':') + 1);
	if (path.starts_with("//")) {

		std::string authority = path.substr(2);
		if (authority.find('/') != std::string::npos) 
			authority = authority.substr(0, authority.find('/'));
		else
			authority = authority.substr(0, std::min(authority.find('?'), authority.find('#')));

		path = path.substr(authority.length() + 2);
	
		std::optional<std::string> userInfo;
		if (authority.find('@') != std::string::npos) {
			userInfo = authority.substr(0, authority.find('@'));
			authority = authority.substr(authority.find('@') + 1);
		}
		this->m_userInfo = userInfo;

		// IPv6 addresses must be surrounded by brackets,
		// so, if the authority (in this stage this should be host + optional port)
		// starts with '[', copy the address into a temp variable and replace it
		// with a random ipv4 address. this shouldn't cause any errors with the parser.
		// once the port is parsed, replace the random ipv4 address with the stored ipv6 address.
		// this is stupid.
		const bool isIpv6 = (authority.find('[') == 0);
		std::optional<std::string> addr = std::nullopt;
		if (isIpv6) {

			if (authority.find(']') == std::string::npos)
				throw BadUriException(ERR_BAD_HOST_FMT.data());

			addr = authority.substr(1, authority.find(']') - 1);
			if(addr->empty())
				throw BadUriException(ERR_BAD_HOST_FMT.data());

			authority = authority.substr(addr->length() + 2);
			authority = ("0.0.0.0" + authority);
				
		}

		std::optional<std::string> port;
		if (authority.find(':') != std::string::npos) {
			port = authority.substr(authority.find(':') + 1);
			authority = authority.substr(0, authority.find(':'));
		}

		// at this point, authority only contains the host or address.
		// if the previous step detected IPv6, replace whatever is currently stored
		// in authority with the stored address.
		if (isIpv6) authority = ("[" + addr.value() + "]");

		if (authority.empty()) throw BadUriException(ERR_URI_HOST_EMPTY.data());
		std::transform(authority.begin(), authority.end(), authority.begin(), [&] (const char ch) -> char {
			return std::tolower(ch);
		});

		this->m_host = authority;

		if (port) {

			it = std::find_if(port.value().begin(), port.value().end(), [&] (const char ch) -> bool {
				return (!std::isdigit(ch));
			});
			if (it != port.value().end()) throw BadUriException(ERR_NON_NUMERICAL_PORT.data());

			std::uint16_t portNum = static_cast<std::uint16_t>(std::strtoul(port.value().c_str(), nullptr, 0));
			this->m_port = portNum;

		}

	}

	if (path.empty() && !this->m_host.has_value()) 
		throw BadUriException(ERR_PATH_COMPONENT_MISSING.data());

	std::string tmp = path; // store the original path (with query string and/or fragment) for later
	if (path.find('?') != std::string::npos) path = path.substr(0, path.find('?'));
	if (path.find('#') != std::string::npos) path = path.substr(0, path.find('#'));
	while (path.starts_with("//")) path = path.substr(1);
	while (path.ends_with('/')) path = path.substr(0, (path.length() - 1));

	std::size_t pos = 0;
	while ((pos = path.find("//", pos)) != std::string::npos) 
		path.replace(pos, 2, "/");

	if (path.empty()) path = "/";
	else if (path[0] != '/') path = ("/" + path);

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

}

Uri::Uri(const Uri& uri) {
	this->operator= (uri);
}

Uri::Uri(Uri&& uri) noexcept {
	this->operator= (std::move(uri));
}

Uri::~Uri() { }

Uri& Uri::operator= (const Uri& uri) {
	
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
	
	if (this->m_scheme != uri.m_scheme) return false;
	if (this->m_userInfo != uri.m_userInfo) return false;
	if (this->m_host != uri.m_host) return false;
	if (this->m_port != uri.m_port) return false;
	if (this->m_path != uri.m_path) return false;
	if (this->m_query != uri.m_query) return false;
	if (this->m_fragment != uri.m_fragment) return false;

	return true;
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

std::string Uri::ToString() const {

	std::ostringstream stream;

	if (this->m_scheme)
		stream << this->m_scheme.value() << ":";

	if (this->m_host) {

		stream << "//";

		if (this->m_userInfo)
			stream << this->m_userInfo.value() << "@";

		stream << this->m_host.value();

		if (this->m_port)
			stream << ":" << this->m_port.value();

	}

	if (this->m_path)
		stream << this->m_path.value();

	if (this->m_query)
		stream << "?" << this->m_query.value();

	if (this->m_fragment)
		stream << "#" << this->m_fragment.value();

	return (stream.str());
}