#include <Vnetworking/Dns/DNS.h>
#include <Vnetworking/Sockets/SocketException.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iphlpapi.h>

#include <string>
#include <optional>
#include <algorithm>
#include <exception>

using namespace Vnetworking;
using namespace Vnetworking::Dns;
using namespace Vnetworking::Sockets;

static inline bool IsNotIpAddress(const std::string& str) noexcept {
	
	// if ip address parse fails, the string is not a valid IPv4 nor IPv6 address.
	try { IpAddress::Parse(str); }
	catch (const std::exception&) {
		return true;
	}

	return false;
}

DnsLookupResult DNS::Resolve(const std::string& hostname) {

	std::optional<std::string> canonicalName = std::nullopt;
	std::vector<std::string> aliases = { };
	std::vector<IpAddress> addresses = { };

	struct addrinfo* result = NULL, * ptr = NULL, hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_flags = AI_CANONNAME;

	int res = getaddrinfo(
		hostname.c_str(),
		NULL,
		&hints,
		&result
	);

	if (res) throw SocketException(WSAGetLastError());

	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

		if (ptr->ai_family != AF_INET && ptr->ai_family != AF_INET6) continue;

		if (ptr->ai_canonname) {
			const std::string tmp { ptr->ai_canonname };
			if (!canonicalName.has_value()) canonicalName = tmp;
			else aliases.push_back(tmp);
		}

		if (ptr->ai_family == AF_INET) {
			struct sockaddr_in* sockaddr = reinterpret_cast<struct sockaddr_in*>(ptr->ai_addr);
			const std::uint32_t addr = sockaddr->sin_addr.S_un.S_addr;
			addresses.push_back({ (addr & 0xFF), ((addr >> 8) & 0xFF), ((addr >> 16) & 0xFF), ((addr >> 24) & 0xFF) });
		}
		else {
			
			struct sockaddr_in6* sockaddr = reinterpret_cast<struct sockaddr_in6*>(ptr->ai_addr);

			std::array<std::uint8_t, 16> bytes;
			std::size_t bytesPos = 0;

			for (int i = 0; i < 8; ++i) {
				const std::uint16_t* word = sockaddr->sin6_addr.u.Word;
				const std::uint8_t l = ((word[i] >> 8) & 0xFF);
				const std::uint8_t h = (word[i] & 0xFF);
				bytes[bytesPos++] = h;
				bytes[bytesPos++] = l;
			}

			addresses.push_back({ bytes });

		}

	}

	freeaddrinfo(result);

	if (IsNotIpAddress(hostname)) {

		if (canonicalName.has_value() && (canonicalName.value() != hostname))
			if (std::find(aliases.begin(), aliases.end(), hostname) == aliases.end())
				aliases.push_back(hostname);

		if (!canonicalName.has_value())
			canonicalName = hostname;

	}

	return { canonicalName.value_or(""), aliases, addresses };
}

DnsLookupResult DNS::Resolve(const IpAddress& ipAddress) {
	return DNS::Resolve(ipAddress.ToString());
}