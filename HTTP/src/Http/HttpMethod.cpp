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

std::string Vnetworking::Http::ToString(const HttpMethod method) {
	if (!s_httpMethods.contains(method))
		throw std::invalid_argument("Invalid HTTP method.");
	return s_httpMethods.at(method);
}