/*
	Vnetworking Library
	Copyright (C) V0idPointer
*/

#ifndef _NE_IPSOCKETADDRESS_H_
#define _NE_IPSOCKETADDRESS_H_

#include <Vnetworking/Exports.h>
#include <Vnetworking/IpAddress.h>
#include <Vnetworking/Sockets/ISocketAddress.h>

namespace Vnetworking::Sockets {

	class VNETCOREAPI IpSocketAddress : public ISocketAddress {

	private:
		AddressFamily m_af;
		IpAddress m_ipAddress;
		Port m_port;

	public:
		IpSocketAddress(void);
		IpSocketAddress(const IpAddress& ipAddress, const Port port);
		IpSocketAddress(const IpSocketAddress& sockaddr);
		IpSocketAddress(IpSocketAddress&& sockaddr) noexcept;
		virtual ~IpSocketAddress(void);

		IpSocketAddress& operator= (const IpSocketAddress& sockaddr);
		IpSocketAddress& operator= (IpSocketAddress&& sockaddr) noexcept;
		bool operator== (const IpSocketAddress& sockaddr) const;

		AddressFamily GetAddressFamily(void) const override;
		IpAddress GetIpAddress(void) const;
		Port GetPort(void) const;

		void SetIpAddress(const IpAddress& ipAddress);
		void SetPort(const Port port);

	};

}

#endif // _NE_IPSOCKETADDRESS_H_