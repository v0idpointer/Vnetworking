/*
	Vnetworking Library
	Copyright (C) V0idPointer
*/

#ifndef _NE_SOCKETTYPE_H_
#define _NE_SOCKETTYPE_H_

#include <Vnetworking/Exports.h>

#include <cstdint>

namespace Vnetworking::Sockets {

	enum class VNETCOREAPI SocketType : std::int32_t { 
	
		STREAM = 1,
		DATAGRAM = 2,
		RAW = 3,
		RDM = 4,
		SEQPACKET = 5,
	
	};

}

#endif // _NE_SOCKETTYPE_H_