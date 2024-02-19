#include <Vnetworking/Http/HttpMethod.h>

#include <string>
#include <unordered_map>
#include <exception>
#include <stdexcept>

using namespace Vnetworking::Http;

static const std::unordered_map<HttpMethod, std::string> s_httpMethods = { 

	{ HttpMethod::GET, "GET" },
	{ HttpMethod::HEAD, "HEAD" },
	{ HttpMethod::POST, "POST" },
	{ HttpMethod::PUT, "PUT" },
	{ HttpMethod::DELETE, "DELETE" },
	{ HttpMethod::CONNECT, "CONNECT" },
	{ HttpMethod::OPTIONS, "OPTIONS" },
	{ HttpMethod::TRACE, "TRACE" },
	{ HttpMethod::PATCH, "PATCH" },

};

static std::unordered_map<HttpMethod, std::string> s_customHttpMethods = { };

constexpr std::string_view ERR_BAD_METHOD = "Invalid HTTP method.";
constexpr std::string_view ERR_TOO_MANY_METHODS = "Too many registered HTTP methods";
constexpr std::string_view ERR_CANNOT_REREGISTER = "Cannot re-register an HTTP method.";
constexpr std::string_view ERR_ALREADY_REGISTERED = "The specified HTTP method is already registered.";
constexpr std::string_view ERR_DOESNT_EXIST = "The specified HTTP method does not exist.";
constexpr std::string_view ERR_CANNOT_UNREGISTER = "The specified HTTP method cannot be unregistered.";

std::string Vnetworking::Http::ToString(const HttpMethod method) {
	
	if (s_httpMethods.contains(method))
		return s_httpMethods.at(method);

	if (s_customHttpMethods.contains(method))
		return s_customHttpMethods.at(method);

	throw std::invalid_argument(ERR_BAD_METHOD.data());
}

HttpMethod Vnetworking::Http::ToMethod(const std::string& method) {

	for (const auto& [key, val] : s_httpMethods) {
		if (val == method)
			return key;
	}

	for (const auto& [key, val] : s_customHttpMethods) {
		if (val == method)
			return key;
	}

	throw std::invalid_argument(ERR_BAD_METHOD.data());
}

HttpMethod Vnetworking::Http::RegisterHttpMethod(const std::string& text) {
	
	std::uint16_t internalId = 0;

	std::uint16_t i = static_cast<std::uint16_t>(s_httpMethods.size() + 1);
	for (i; i < UINT16_MAX; ++i) {
		const HttpMethod method = static_cast<HttpMethod>(i);
		if (!s_customHttpMethods.contains(method)) {
			internalId = i;
			break;
		}
	}

	if (i == 0)
		throw std::invalid_argument(ERR_TOO_MANY_METHODS.data());

	for (const auto& [key, val] : s_httpMethods) {
		if (val == text)
			throw std::invalid_argument(ERR_CANNOT_REREGISTER.data());
	}

	for (const auto& [key, val] : s_customHttpMethods) {
		if (val == text)
			throw std::invalid_argument(ERR_ALREADY_REGISTERED.data());
	}

	HttpMethod method = static_cast<HttpMethod>(internalId);
	s_customHttpMethods.insert({ method, text });

	return method;
}

void Vnetworking::Http::UnregisterHttpMethod(const HttpMethod method) { 

	if (s_httpMethods.contains(method))
		throw std::invalid_argument(ERR_CANNOT_UNREGISTER.data());

	if (!s_customHttpMethods.contains(method))
		throw std::invalid_argument(ERR_DOESNT_EXIST.data());

	s_customHttpMethods.erase(method);

}