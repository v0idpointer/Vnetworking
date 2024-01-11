#include <Vnetworking/Http/HttpVersion.h>

#include <string>
#include <unordered_map>
#include <exception>
#include <stdexcept>

using namespace Vnetworking::Http;

static const std::unordered_map<HttpVersion, std::string> s_httpVersions = { 

	{ HttpVersion::HTTP_1_0, "HTTP/1.0" },
	{ HttpVersion::HTTP_1_1, "HTTP/1.1" },
	{ HttpVersion::HTTP_2_0, "HTTP/2.0" },
	{ HttpVersion::HTTP_3_0, "HTTP/3.0" },

};

std::string Vnetworking::Http::ToString(const HttpVersion version) {
	if (!s_httpVersions.contains(version))
		throw std::invalid_argument("Invalid and/or unsupported HTTP version.");
	return s_httpVersions.at(version);
}