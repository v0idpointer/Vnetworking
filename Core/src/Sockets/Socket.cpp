#include <Vnetworking/Sockets/Socket.h>
#include <Vnetworking/Sockets/IpSocketAddress.h>
#include <Vnetworking/Sockets/SocketException.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iphlpapi.h>

#include <unordered_map>
#include <exception>
#include <stdexcept>

#pragma comment (lib, "WS2_32.lib")

using namespace Vnetworking::Sockets;

// error messages for Send and Receive functions:
constexpr std::string_view ERR_OFFSET_LESS_THAN_ZERO = "'offset' is less than zero.";
constexpr std::string_view ERR_OFFSET_GREATER_THAN_BUFFERSIZE = "'offset' is greater than the buffer size.";
constexpr std::string_view ERR_SIZE_LESS_THAN_ZERO = "'size' is less that zero.";
constexpr std::string_view ERR_SIZE_GREATER_THAN_BUFFERSIZE_MINUS_OFFSET = "'size' is greater than the buffer size minus 'offset'.";

constexpr std::string_view ERR_BAD_ISOCKETADDRESS_IMPL = "Invalid ISocketAddress implementation.";
constexpr std::string_view ERR_BAD_ADDRESSFAMILY = "Invalid AddressFamily and/or ISocketAddress implementation.";

static const std::unordered_map<AddressFamily, std::int32_t> s_addressFamilies = { 

	{ AddressFamily::UNSPECIFIED, NULL },
	{ AddressFamily::IPV4, AF_INET },
	{ AddressFamily::IPV6, AF_INET6 },

};

static const std::unordered_map<SocketType, std::int32_t> s_socketTypes = {

	{ SocketType::STREAM, SOCK_STREAM },
	{ SocketType::DATAGRAM, SOCK_DGRAM },
	{ SocketType::RAW, SOCK_RAW },
	{ SocketType::RDM, SOCK_RDM },
	{ SocketType::SEQPACKET, SOCK_SEQPACKET },

};

static const std::unordered_map<ProtocolType, std::int32_t> s_protocolTypes = {

	{ ProtocolType::UNSPECIFIED, NULL },
	{ ProtocolType::TCP, IPPROTO_TCP },
	{ ProtocolType::UDP, IPPROTO_UDP },

};

static const std::unordered_map<SocketFlags, std::int32_t> s_socketFlags = { 

	{ SocketFlags::NONE, NULL },
	{ SocketFlags::OUT_OF_BAND, MSG_OOB },
	{ SocketFlags::PEEK, MSG_PEEK },
	{ SocketFlags::DONT_ROUTE, MSG_DONTROUTE },

};

Socket::Socket(const NativeSocket_t socket, const AddressFamily af, const SocketType type, const ProtocolType proto) 
	: m_socket(socket), m_af(af), m_type(type), m_proto(proto) { }

Socket::Socket(const AddressFamily af, const SocketType type, const ProtocolType proto) :
	m_socket(INVALID_SOCKET_HANDLE), m_af(af), m_type(type), m_proto(proto) {
	
	std::int32_t addressFamily = -1;
	std::int32_t socketType = -1;
	std::int32_t protocolType = -1;

	if (s_addressFamilies.contains(af)) addressFamily = s_addressFamilies.at(af);
	if (s_socketTypes.contains(type)) socketType = s_socketTypes.at(type);
	if (s_protocolTypes.contains(proto)) protocolType = s_protocolTypes.at(proto);

	this->m_socket = socket(addressFamily, socketType, protocolType);
	if (this->m_socket == INVALID_SOCKET_HANDLE)
		throw SocketException(WSAGetLastError());

}

Socket::Socket(Socket&& other) noexcept {
	this->m_socket = INVALID_SOCKET_HANDLE;
	this->operator= (std::move(other));
}

Socket::~Socket() { 
	this->Close();
}

Socket& Socket::operator= (Socket&& socket) noexcept {

	if (this->m_socket != INVALID_SOCKET_HANDLE)
		this->Close();

	this->m_af = socket.m_af;
	this->m_type = socket.m_type;
	this->m_proto = socket.m_proto;
	this->m_socket = socket.m_socket;

	socket.m_socket = INVALID_SOCKET_HANDLE;

	return static_cast<Socket&>(*this);
}

bool Socket::operator== (const Socket& socket) const {

	if (this->m_af != socket.m_af) return false;
	if (this->m_type != socket.m_type) return false;
	if (this->m_proto == socket.m_proto) return false;
	if (this->m_socket == socket.m_socket) return false;

	return true;
}

AddressFamily Socket::GetAddressFamily() const {
	return this->m_af;
}

SocketType Socket::GetSocketType() const {
	return this->m_type;
}

ProtocolType Socket::GetProtocolType() const {
	return this->m_proto;
}

NativeSocket_t Socket::GetNativeSocketHandle() const {
	return this->m_socket;
}

void Socket::Close() {
	
	if (this->m_socket != INVALID_SOCKET_HANDLE) {
		closesocket(this->m_socket);
		this->m_socket = INVALID_SOCKET_HANDLE;
	}

}

void Socket::Shutdown(const ShutdownSocket how) const {

	int sd = 0;
	switch (how) {

	case ShutdownSocket::RECEIVE:
		sd = SD_RECEIVE;
		break;

	case ShutdownSocket::SEND:
		sd = SD_SEND;
		break;

	case ShutdownSocket::BOTH:
		sd = SD_BOTH;
		break;

	}

	if (shutdown(this->m_socket, sd))
		throw SocketException(WSAGetLastError());

}

// if successful, this function returns a pointer to addrinfo struct. 
// once no longer needed, the object should be deleted using freeaddrinfo().
// if the provided ISocketAddress implementation is for an unsupported address family, std::invalid_argument is thrown. 
// if the provided ISocketAddress implementation is not valid for a specific address family, 
// (ex. only IpSocketAddress is valid for IPV4 and IPV6. any other impl. would be considered invalid) std::invalid_argument is thrown.
static struct addrinfo* CreateNativeAddrinfo(const Socket& socket, const ISocketAddress& sockaddr) {

	if (sockaddr.GetAddressFamily() == AddressFamily::IPV4 || sockaddr.GetAddressFamily() == AddressFamily::IPV6) {

		const IpSocketAddress* pIpSockaddr = dynamic_cast<const IpSocketAddress*>(&sockaddr);
		if (pIpSockaddr == nullptr)
			throw std::invalid_argument(ERR_BAD_ISOCKETADDRESS_IMPL.data());

		struct addrinfo* result = NULL, * ptr = NULL, hints;
		ZeroMemory(&hints, sizeof(hints));
		hints.ai_family = s_addressFamilies.at(socket.GetAddressFamily());
		hints.ai_socktype = s_socketTypes.at(socket.GetSocketType());
		hints.ai_protocol = s_protocolTypes.at(socket.GetProtocolType());
		hints.ai_flags = AI_PASSIVE;

		int res = getaddrinfo(
			pIpSockaddr->GetIpAddress().ToString().c_str(),
			std::to_string(pIpSockaddr->GetPort()).c_str(),
			&hints,
			&result
		);

		if (res) throw SocketException(WSAGetLastError());

		return result;
	}

	throw std::invalid_argument(ERR_BAD_ADDRESSFAMILY.data());
	return nullptr;
}

static void NativeSockaddrToISocketAddress(const Socket&, const struct sockaddr* source, ISocketAddress& destination) {

	if (destination.GetAddressFamily() == AddressFamily::IPV4 || destination.GetAddressFamily() == AddressFamily::IPV6) {

		IpSocketAddress* pDestination = dynamic_cast<IpSocketAddress*>(&destination);
		if (pDestination == nullptr)
			throw std::invalid_argument(ERR_BAD_ISOCKETADDRESS_IMPL.data());

		if (source->sa_family == AF_INET) {
			const struct sockaddr_in* sockaddr = reinterpret_cast<const struct sockaddr_in*>(source);
			const std::uint32_t a = sockaddr->sin_addr.S_un.S_addr;
			const std::uint16_t p = sockaddr->sin_port;
			pDestination->SetIpAddress({ (a & 0xFF), ((a >> 8) & 0xFF), ((a >> 16) & 0xFF), ((a >> 24) & 0xFF) });
			pDestination->SetPort(static_cast<Vnetworking::Port>(((p & 0xFF) << 8) | ((p & 0xFF00) >> 8)));
		}
		else {

			const struct sockaddr_in6* sockaddr = reinterpret_cast<const struct sockaddr_in6*>(source);
			const std::uint16_t* a = sockaddr->sin6_addr.u.Word;
			const std::uint16_t p = sockaddr->sin6_port;
			std::array<std::uint8_t, 16> bytes;
			std::size_t bytesPos = 0;

			for (int i = 0; i < 8; ++i) {
				const std::uint16_t* word = sockaddr->sin6_addr.u.Word;
				const std::uint8_t l = ((word[i] >> 8) & 0xFF);
				const std::uint8_t h = (word[i] & 0xFF);
				bytes[bytesPos++] = h;
				bytes[bytesPos++] = l;
			}

			pDestination->SetIpAddress({ bytes });
			pDestination->SetPort(static_cast<Vnetworking::Port>(((p & 0xFF) << 8) | ((p & 0xFF00) >> 8)));

		}

		return;
	}

	throw std::invalid_argument(ERR_BAD_ADDRESSFAMILY.data());
	return;
}

static std::int32_t CreateFlags(const SocketFlags flags) noexcept {

	std::int32_t nf = 0;
	for (const auto [key, val] : s_socketFlags) {
		if (static_cast<bool>(flags & key)) nf |= val;
	}

	return nf;
}

void Socket::Bind(const ISocketAddress& sockaddr) const {

	struct addrinfo* result = CreateNativeAddrinfo(static_cast<const Socket&>(*this), sockaddr);

	int res = bind(this->m_socket, result->ai_addr, static_cast<int>(result->ai_addrlen));
	if (res) {
		freeaddrinfo(result);
		throw SocketException(WSAGetLastError());
	}

	freeaddrinfo(result);

}

void Socket::Connect(const ISocketAddress& sockaddr) const {

	struct addrinfo* result = CreateNativeAddrinfo(static_cast<const Socket&>(*this), sockaddr);

	int res = connect(this->m_socket, result->ai_addr, static_cast<int>(result->ai_addrlen));
	if (res) {
		freeaddrinfo(result);
		throw SocketException(WSAGetLastError());
	}

	freeaddrinfo(result);

}

void Socket::Listen() const {
	this->Listen(SOMAXCONN);
}

void Socket::Listen(const std::int32_t backlog) const {
	if (listen(this->m_socket, backlog) == SOCKET_ERROR)
		throw SocketException(WSAGetLastError());
}

Socket Socket::Accept() const {

	NativeSocket_t client = accept(this->m_socket, nullptr, nullptr);
	if (client == INVALID_SOCKET)
		throw SocketException(WSAGetLastError());

	return Socket(client, this->GetAddressFamily(), this->GetSocketType(), this->GetProtocolType());
}

std::int32_t Socket::Send(const std::vector<std::uint8_t>& data, const std::int32_t offset, const std::int32_t size, const SocketFlags flags) const {

	if (offset < 0) 
		throw std::out_of_range(ERR_OFFSET_LESS_THAN_ZERO.data());

	if (offset > data.size()) 
		throw std::out_of_range(ERR_OFFSET_GREATER_THAN_BUFFERSIZE.data());
	
	if (size < 0) 
		throw std::out_of_range(ERR_SIZE_LESS_THAN_ZERO.data());
	
	if (size > (data.size() - offset)) 
		throw std::out_of_range(ERR_SIZE_GREATER_THAN_BUFFERSIZE_MINUS_OFFSET.data());

	const char* buffer = (reinterpret_cast<const char*>(data.data()) + offset);
	
	std::int32_t sent = send(this->m_socket, buffer, size, CreateFlags(flags));
	if (sent == SOCKET_ERROR)
		throw SocketException(WSAGetLastError());

	return sent;
}

std::int32_t Socket::Send(const std::vector<std::uint8_t>& data, const std::int32_t size, const SocketFlags flags) const {
	return this->Send(data, 0, size, flags);
}

std::int32_t Socket::Send(const std::vector<std::uint8_t>& data, const std::int32_t size) const {
	return this->Send(data, 0, size, SocketFlags::NONE);
}

std::int32_t Socket::Receive(std::vector<std::uint8_t>& data, const std::int32_t offset, const std::int32_t size, const SocketFlags flags) const {

	if (offset < 0)
		throw std::out_of_range(ERR_OFFSET_LESS_THAN_ZERO.data());

	if (offset > data.size())
		throw std::out_of_range(ERR_OFFSET_GREATER_THAN_BUFFERSIZE.data());

	if (size < 0)
		throw std::out_of_range(ERR_SIZE_LESS_THAN_ZERO.data());

	if (size > (data.size() - offset))
		throw std::out_of_range(ERR_SIZE_GREATER_THAN_BUFFERSIZE_MINUS_OFFSET.data());

	char* buffer = (reinterpret_cast<char*>(data.data()) + offset);

	std::int32_t read = recv(this->m_socket, buffer, size, CreateFlags(flags));
	if (read == SOCKET_ERROR)
		throw SocketException(WSAGetLastError());

	return read;
}

std::int32_t Socket::Receive(std::vector<std::uint8_t>& data, const std::int32_t size, const SocketFlags flags) const {
	return this->Receive(data, 0, size, flags);
}

std::int32_t Socket::Receive(std::vector<std::uint8_t>& data, const std::int32_t size) const {
	return this->Receive(data, 0, size, SocketFlags::NONE);
}

std::int32_t Socket::SendTo(
	const std::vector<std::uint8_t>& data,
	const std::int32_t offset,
	const std::int32_t size,
	const SocketFlags flags,
	const ISocketAddress& sockaddr
) const {
	
	if (offset < 0)
		throw std::out_of_range(ERR_OFFSET_LESS_THAN_ZERO.data());

	if (offset > data.size())
		throw std::out_of_range(ERR_OFFSET_GREATER_THAN_BUFFERSIZE.data());

	if (size < 0)
		throw std::out_of_range(ERR_SIZE_LESS_THAN_ZERO.data());

	if (size > (data.size() - offset))
		throw std::out_of_range(ERR_SIZE_GREATER_THAN_BUFFERSIZE_MINUS_OFFSET.data());

	const char* buffer = (reinterpret_cast<const char*>(data.data()) + offset);

	struct addrinfo* result = CreateNativeAddrinfo(static_cast<const Socket&>(*this), sockaddr);
	std::int32_t sent = sendto(this->m_socket, buffer, size, CreateFlags(flags), result->ai_addr, static_cast<int>(result->ai_addrlen));
	if (sent == SOCKET_ERROR) {
		freeaddrinfo(result);
		throw SocketException(WSAGetLastError());
	}

	freeaddrinfo(result);

	return sent;
}

std::int32_t Socket::SendTo(
	const std::vector<std::uint8_t>& data,
	const std::int32_t size,
	const SocketFlags flags,
	const ISocketAddress& sockaddr
) const {
	return this->SendTo(data, 0, size, flags, sockaddr);
}

std::int32_t Socket::SendTo(
	const std::vector<std::uint8_t>& data,
	const std::int32_t size,
	const ISocketAddress& sockaddr
) const {
	return this->SendTo(data, size, SocketFlags::NONE, sockaddr);
}

std::int32_t Socket::ReceiveFrom(
	std::vector<std::uint8_t>& data, 
	const std::int32_t offset, 
	const std::int32_t size, 
	const SocketFlags flags, 
	ISocketAddress& sockaddr
) const {
	
	if (offset < 0)
		throw std::out_of_range(ERR_OFFSET_LESS_THAN_ZERO.data());

	if (offset > data.size())
		throw std::out_of_range(ERR_OFFSET_GREATER_THAN_BUFFERSIZE.data());

	if (size < 0)
		throw std::out_of_range(ERR_SIZE_LESS_THAN_ZERO.data());

	if (size > (data.size() - offset))
		throw std::out_of_range(ERR_SIZE_GREATER_THAN_BUFFERSIZE_MINUS_OFFSET.data());

	char* buffer = (reinterpret_cast<char*>(data.data()) + offset);

	struct sockaddr sender;
	std::int32_t senderLen = sizeof(sender);

	std::int32_t read = recvfrom(this->m_socket, buffer, size, CreateFlags(flags), &sender, &senderLen);
	if (read == SOCKET_ERROR)
		throw SocketException(WSAGetLastError());

	NativeSockaddrToISocketAddress(static_cast<const Socket&>(*this), &sender, sockaddr);

	return read;
}

std::int32_t Socket::ReceiveFrom(
	std::vector<std::uint8_t>& data,
	const std::int32_t size,
	const SocketFlags flags,
	ISocketAddress& sockaddr
) const {
	return this->ReceiveFrom(data, 0, size, flags, sockaddr);
}

std::int32_t Socket::ReceiveFrom(
	std::vector<std::uint8_t>& data,
	const std::int32_t size,
	ISocketAddress& sockaddr
) const {
	return this->ReceiveFrom(data, size, SocketFlags::NONE, sockaddr);
}

std::int32_t Socket::GetAvailableBytes() const {

	u_long argp = 0;
	int res = ioctlsocket(this->m_socket, FIONREAD, &argp);
	if (res == SOCKET_ERROR)
		throw SocketException(WSAGetLastError());

	return static_cast<std::int32_t>(argp);
}

void Socket::GetSocketAddress(ISocketAddress& sockaddr) const {

	struct sockaddr sockName;
	std::int32_t sockNameLen = sizeof(sockName);

	if (getsockname(this->m_socket, &sockName, &sockNameLen))
		throw SocketException(WSAGetLastError());

	NativeSockaddrToISocketAddress(static_cast<const Socket&>(*this), &sockName, sockaddr);

}

void Socket::GetPeerAddress(ISocketAddress& sockaddr) const {

	struct sockaddr peerName;
	std::int32_t peerNameLen = sizeof(peerName);

	if (getpeername(this->m_socket, &peerName, &peerNameLen))
		throw SocketException(WSAGetLastError());

	NativeSockaddrToISocketAddress(static_cast<const Socket&>(*this), &peerName, sockaddr);

}