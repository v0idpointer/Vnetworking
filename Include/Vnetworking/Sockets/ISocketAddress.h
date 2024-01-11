/*
	Vnetworking Library
	Copyright (C) V0idPointer
*/

#ifndef _NE_ISOCKETADDRESS_H_
#define _NE_ISOCKETADDRESS_H_

#include <Vnetworking/Exports.h>
#include <Vnetworking/Sockets/AddressFamily.h>

namespace Vnetworking::Sockets {

	class VNETCOREAPI ISocketAddress {
	public:
		virtual ~ISocketAddress(void) { };
		virtual AddressFamily GetAddressFamily(void) const = 0;
	};

}

#endif // _NE_ISOCKETADDRESS_H_