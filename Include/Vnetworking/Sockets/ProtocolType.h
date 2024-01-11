/*
	Vnetworking Library
	Copyright (C) V0idPointer
*/

#ifndef _NE_PROTOCOLTYPE_H_
#define _NE_PROTOCOLTYPE_H_

#include <Vnetworking/Exports.h>

#include <cstdint>

namespace Vnetworking::Sockets {

	enum class VNETCOREAPI ProtocolType : std::int32_t { 
	
		UNSPECIFIED,
		TCP,
		UDP,

	};

}

#endif // _NE_PROTOCOLTYPE_H_