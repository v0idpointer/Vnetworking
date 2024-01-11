#include <Vnetworking/IpAddress.h>

#include <string>
#include <format>
#include <sstream>
#include <regex>
#include <exception>
#include <algorithm>

#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

using namespace Vnetworking;

static const std::regex s_ipv4Regex(R"((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?))");
static const std::regex s_ipv6Regex(R"((([0-9a-fA-F]{1,4}:){7}[0-9a-fA-F]{1,4}|([0-9a-fA-F]{1,4}:){1,7}:|([0-9a-fA-F]{1,4}:){1,6}[0-9a-fA-F]{1,4}|([0-9a-fA-F]{1,4}:){1,5}(:[0-9a-fA-F]{1,4}){1,2}|([0-9a-fA-F]{1,4}:){1,4}(:[0-9a-fA-F]{1,4}){1,3}|([0-9a-fA-F]{1,4}:){1,3}(:[0-9a-fA-F]{1,4}){1,4}|([0-9a-fA-F]{1,4}:){1,2}(:[0-9a-fA-F]{1,4}){1,5}|[0-9a-fA-F]{1,4}:(:[0-9a-fA-F]{1,4}){1,6}|:((:[0-9a-fA-F]{1,4}){1,7}|:)|fe80:(:[0-9a-fA-F]{0,4}){0,4}%[0-9a-zA-Z]{1,}|::(ffff(:0{1,4}){0,1}:){0,1}((25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])\\.){3,3}(25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])|([0-9a-fA-F]{1,4}:){1,4}:((25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])\\.){3,3}(25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])))");

IpAddress::IpAddress() : IpAddress(0, 0, 0, 0) { }

IpAddress::IpAddress(const std::uint8_t aa, const std::uint8_t bb, const std::uint8_t cc, const std::uint8_t dd) { 
	
	this->m_isVersion6 = false;
	
	memset(this->m_bytes.data(), 0, this->m_bytes.size());
	this->m_bytes[0] = aa;
	this->m_bytes[1] = bb;
	this->m_bytes[2] = cc;
	this->m_bytes[3] = dd;

}

IpAddress::IpAddress(const std::array<std::uint8_t, 4>& address) 
	: IpAddress(address[0], address[1], address[2], address[3]) { }

IpAddress::IpAddress(const std::array<std::uint8_t, 16>& address) {
	this->m_isVersion6 = true;
	memcpy_s(this->m_bytes.data(), this->m_bytes.size(), address.data(), address.size());
}

IpAddress::IpAddress(const IpAddress& address) {
	this->operator= (address);
}

IpAddress::IpAddress(IpAddress&& address) noexcept {
	this->operator= (std::move(address));
}

IpAddress::~IpAddress() { }

IpAddress& IpAddress::operator= (const IpAddress& address) {
	this->m_isVersion6 = address.m_isVersion6;
	memcpy_s(this->m_bytes.data(), this->m_bytes.size(), address.m_bytes.data(), address.m_bytes.size());
	return static_cast<IpAddress&>(*this);
}

IpAddress& IpAddress::operator= (IpAddress&& address) noexcept {
	this->m_isVersion6 = address.m_isVersion6;
	this->m_bytes = std::move(address.m_bytes);
	return static_cast<IpAddress&>(*this);
}

bool IpAddress::operator== (const IpAddress& address) const {
	if (this->m_isVersion6 != address.m_isVersion6) return false;
	return (memcmp(this->m_bytes.data(), address.m_bytes.data(), this->m_bytes.size()) == 0); 
}

bool IpAddress::IsVersion6() const {
	return this->m_isVersion6;
}

std::string IpAddress::ToString() const {

	if (!this->m_isVersion6)
		return std::format(
			"{0}.{1}.{2}.{3}",
			this->m_bytes[0],
			this->m_bytes[1],
			this->m_bytes[2],
			this->m_bytes[3]
		);

	// yup, this is shit:
	std::ostringstream stream;
	std::uint8_t counter = 0;
	for (size_t i = 0; i < this->m_bytes.size(); ++i) {
		
		stream << std::format("{0:02x}", this->m_bytes[i]);

		if (counter++ == 1) {
			stream << ":";
			counter = 0;
		}

	}

	std::string str = stream.str();
	str = str.substr(0, str.length() - 1);
	stream.clear();

	return str;
}

const std::array<std::uint8_t, 16>& IpAddress::GetAddressBytes() const {
	return this->m_bytes;
}

bool IpAddress::IsPublicAddress() const {
	return (!this->IsPrivateAddress());
}

static constexpr inline bool IsPrivateAddressA(const std::uint32_t addr) noexcept {
	return (((addr >> 24) & 0xFF) == 10);
}

static constexpr inline bool IsPrivateAddressB(const std::uint32_t addr) noexcept {
	const std::uint8_t aa = ((addr >> 24) & 0xFF);
	const std::uint8_t bb = ((addr >> 16) & 0xFF);
	return ((aa == 172) && ((bb >= 16) && (bb <= 32)));
}

static constexpr inline bool IsPrivateAddressC(const std::uint32_t addr) noexcept {
	const std::uint8_t aa = ((addr >> 24) & 0xFF);
	const std::uint8_t bb = ((addr >> 16) & 0xFF);
	return ((aa == 192) && (bb == 168));
}

static constexpr inline bool IsPrivateAddressAPIPA(const std::uint32_t addr) noexcept {
	const std::uint8_t aa = ((addr >> 24) & 0xFF);
	const std::uint8_t bb = ((addr >> 16) & 0xFF);
	return ((aa == 169) && (bb == 254));
}

static constexpr inline bool IsPrivateAddressLocalhost(const std::uint32_t addr) noexcept {
	return (addr == (127 << 24 | 1));
}

bool IpAddress::IsPrivateAddress() const {

	if (this->m_isVersion6) {
		if ((this->m_bytes[0] == 0xFC) || (this->m_bytes[0] == 0xFD)) return true;
		if ((this->m_bytes[0] == 0xFE) && (this->m_bytes[1] == 0x80)) return true;
		if (this->operator== (IpAddress::Localhost6())) return true;
		return false;
	}

	const std::uint32_t addr = ( (this->m_bytes[0] << 24) | (this->m_bytes[1] << 16) | (this->m_bytes[2] << 8) | this->m_bytes[3]);

	return (
		IsPrivateAddressA(addr) ||
		IsPrivateAddressB(addr) ||
		IsPrivateAddressC(addr) ||
		IsPrivateAddressAPIPA(addr) ||
		IsPrivateAddressLocalhost(addr)
	);
}

IpAddress IpAddress::Parse(const std::string& ipAddress) {

	const bool isVersion4 = std::regex_match(ipAddress, s_ipv4Regex);
	const bool isVersion6 = std::regex_match(ipAddress, s_ipv6Regex);

	if (!isVersion4 && !isVersion6)
		throw std::invalid_argument("The provided address is neither a valid IPv4 address nor an IPv6 address.");

	// parse IPv4 address:
	if (isVersion4) {

		struct sockaddr_in sockaddr;
		std::int32_t sockaddrLen = sizeof(sockaddr);

		int res = WSAStringToAddressA(
			const_cast<LPSTR>(ipAddress.c_str()),
			AF_INET,
			NULL,
			reinterpret_cast<LPSOCKADDR>(&sockaddr),
			&sockaddrLen
		);

		if (res == SOCKET_ERROR)
			throw std::invalid_argument("The provided address is not a valid IPv4 address.");

		const std::uint32_t addr = sockaddr.sin_addr.S_un.S_addr;

		return { (addr & 0xFF), ((addr >> 8) & 0xFF), ((addr >> 16) & 0xFF), ((addr >> 24) & 0xFF) };
	}

	// parse IPv6 address:

	struct sockaddr_in6 sockaddr;
	std::int32_t sockaddrLen = sizeof(sockaddr);

	int res = WSAStringToAddressA(
		const_cast<LPSTR>(ipAddress.c_str()),
		AF_INET6,
		NULL,
		reinterpret_cast<LPSOCKADDR>(&sockaddr),
		&sockaddrLen
	);

	if (res == SOCKET_ERROR)
		throw std::invalid_argument("The provided address is not a valid IPv6 address.");

	std::array<std::uint8_t, 16> bytes = { 0 };
	std::int32_t bytesPos = 0;

	for (int i = 0; i < 8; ++i) {
		const std::uint16_t* word = sockaddr.sin6_addr.u.Word;
		const std::uint8_t l = ((word[i] >> 8) & 0xFF);
		const std::uint8_t h = (word[i] & 0xFF);
		bytes[bytesPos++] = h;
		bytes[bytesPos++] = l;
	}

	return { bytes };
}