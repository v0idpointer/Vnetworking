/*
	Vnetworking Library
	Copyright (C) V0idPointer
*/

#ifndef _NE_SOCKET_H_
#define _NE_SOCKET_H_

#include <Vnetworking/Exports.h>
#include <Vnetworking/Sockets/AddressFamily.h>
#include <Vnetworking/Sockets/SocketType.h>
#include <Vnetworking/Sockets/ProtocolType.h>
#include <Vnetworking/Sockets/ShutdownSocket.h>
#include <Vnetworking/Sockets/ISocketAddress.h>
#include <Vnetworking/Sockets/SocketFlags.h>

#include <cstdint>
#include <vector>

namespace Vnetworking::Sockets {

	typedef std::uint64_t NativeSocket_t;

	constexpr NativeSocket_t INVALID_SOCKET_HANDLE = (NativeSocket_t)(~0);

	class VNETCOREAPI Socket { 
	
	private:
		const AddressFamily m_af;
		const SocketType m_type;
		const ProtocolType m_proto;

		NativeSocket_t m_socket;

	private:
		Socket(const NativeSocket_t, const AddressFamily, const SocketType, const ProtocolType);

	public:
		Socket(const AddressFamily af, const SocketType type, const ProtocolType proto);
		Socket(const Socket&) = delete;
		Socket(Socket&& other) noexcept;
		virtual ~Socket(void);

		Socket& operator= (const Socket&) = delete;
		bool operator== (const Socket& socket) const;

		AddressFamily GetAddressFamily(void) const;
		SocketType GetSocketType(void) const;
		ProtocolType GetProtocolType(void) const;

		NativeSocket_t GetNativeSocketHandle(void) const;

		void Close(void);
		void Shutdown(const ShutdownSocket how);
	
		void Bind(const ISocketAddress& sockaddr);
		void Connect(const ISocketAddress& sockaddr);

		void Listen(void);
		void Listen(const std::int32_t backlog);
		Socket Accept(void);

		std::int32_t Send(const std::vector<std::uint8_t>& data, const std::int32_t offset, const std::int32_t size, const SocketFlags flags);
		std::int32_t Send(const std::vector<std::uint8_t>& data, const std::int32_t size, const SocketFlags flags);
		std::int32_t Send(const std::vector<std::uint8_t>& data, const std::int32_t size);

		std::int32_t Receive(std::vector<std::uint8_t>& data, const std::int32_t offset, const std::int32_t size, const SocketFlags flags);
		std::int32_t Receive(std::vector<std::uint8_t>& data, const std::int32_t size, const SocketFlags flags);
		std::int32_t Receive(std::vector<std::uint8_t>& data, const std::int32_t size);

		std::int32_t SendTo(const std::vector<std::uint8_t>& data, const std::int32_t offset, const std::int32_t size, const SocketFlags flags, const ISocketAddress& sockaddr);
		std::int32_t SendTo(const std::vector<std::uint8_t>& data, const std::int32_t size, const SocketFlags flags, const ISocketAddress& sockaddr);
		std::int32_t SendTo(const std::vector<std::uint8_t>& data, const std::int32_t size, const ISocketAddress& sockaddr);

		std::int32_t ReceiveFrom(std::vector<std::uint8_t>& data, const std::int32_t offset, const std::int32_t size, const SocketFlags flags, ISocketAddress& sockaddr);
		std::int32_t ReceiveFrom(std::vector<std::uint8_t>& data, const std::int32_t size, const SocketFlags flags, ISocketAddress& sockaddr);
		std::int32_t ReceiveFrom(std::vector<std::uint8_t>& data, const std::int32_t size, ISocketAddress& sockaddr);

		std::int32_t GetAvailableBytes(void);

	};

}

#endif // _NE_SOCKET_H_