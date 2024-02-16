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
#include <Vnetworking/Sockets/PollEvents.h>

#include <cstdint>
#include <span>

namespace Vnetworking::Sockets {

	typedef std::uint64_t NativeSocket_t;

	constexpr NativeSocket_t INVALID_SOCKET_HANDLE = (NativeSocket_t)(~0);

	class VNETCOREAPI Socket { 
	
	private:
		AddressFamily m_af;
		SocketType m_type;
		ProtocolType m_proto;

		NativeSocket_t m_socket;

	private:
		Socket(const NativeSocket_t, const AddressFamily, const SocketType, const ProtocolType);

	public:
		Socket(const AddressFamily af, const SocketType type, const ProtocolType proto);
		Socket(const Socket&) = delete;
		Socket(Socket&& other) noexcept;
		virtual ~Socket(void);

		Socket& operator= (const Socket&) = delete;
		Socket& operator= (Socket&& socket) noexcept;
		bool operator== (const Socket& socket) const;

		AddressFamily GetAddressFamily(void) const;
		SocketType GetSocketType(void) const;
		ProtocolType GetProtocolType(void) const;

		NativeSocket_t GetNativeSocketHandle(void) const;

		void Close(void);
		void Shutdown(const ShutdownSocket how) const;
	
		void Bind(const ISocketAddress& sockaddr) const;
		void Connect(const ISocketAddress& sockaddr) const;

		void Listen(void) const;
		void Listen(const std::int32_t backlog) const;
		Socket Accept(void) const;

		std::int32_t Send(const std::span<const std::uint8_t>& data, const std::int32_t offset, const std::int32_t size, const SocketFlags flags) const;
		std::int32_t Send(const std::span<const std::uint8_t>& data, const std::int32_t size, const SocketFlags flags) const;
		std::int32_t Send(const std::span<const std::uint8_t>& data, const std::int32_t size) const;

		std::int32_t Receive(const std::span<std::uint8_t>& data, const std::int32_t offset, const std::int32_t size, const SocketFlags flags) const;
		std::int32_t Receive(const std::span<std::uint8_t>& data, const std::int32_t size, const SocketFlags flags) const;
		std::int32_t Receive(const std::span<std::uint8_t>& data, const std::int32_t size) const;

		std::int32_t SendTo(const std::span<const std::uint8_t>& data, const std::int32_t offset, const std::int32_t size, const SocketFlags flags, const ISocketAddress& sockaddr) const;
		std::int32_t SendTo(const std::span<const std::uint8_t>& data, const std::int32_t size, const SocketFlags flags, const ISocketAddress& sockaddr) const;
		std::int32_t SendTo(const std::span<const std::uint8_t>& data, const std::int32_t size, const ISocketAddress& sockaddr) const;

		std::int32_t ReceiveFrom(const std::span<std::uint8_t>& data, const std::int32_t offset, const std::int32_t size, const SocketFlags flags, ISocketAddress& sockaddr) const;
		std::int32_t ReceiveFrom(const std::span<std::uint8_t>& data, const std::int32_t size, const SocketFlags flags, ISocketAddress& sockaddr) const;
		std::int32_t ReceiveFrom(const std::span<std::uint8_t>& data, const std::int32_t size, ISocketAddress& sockaddr) const;

		std::int32_t GetAvailableBytes(void) const;

		void GetSocketAddress(ISocketAddress& sockaddr) const;
		void GetPeerAddress(ISocketAddress& sockaddr) const;

		bool Poll(const PollEvents pollEvent, const std::int32_t timeout) const;

	};

}

#endif // _NE_SOCKET_H_