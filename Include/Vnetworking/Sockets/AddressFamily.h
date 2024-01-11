/*
	Vnetworking Library
	Copyright (C) V0idPointer
*/

#ifndef _NE_ADDRESSFAMILY_H_
#define _NE_ADDRESSFAMILY_H_

#include <Vnetworking/Exports.h>

#include <cstdint>

namespace Vnetworking::Sockets { 

	enum class VNETCOREAPI AddressFamily : std::int32_t { 
	
		UNSPECIFIED,
		IPV4,
		IPV6,
	
	};

}

#endif // _NE_ADDRESSFAMILY_H_