/*
	Vnetworking Library
	Copyright (C) V0idPointer
*/

#ifndef _NE_SHUTDOWNSOCKET_H_
#define _NE_SHUTDOWNSOCKET_H_

#include <Vnetworking/Exports.h>

#include <cstdint>

namespace Vnetworking::Sockets {

	enum class VNETCOREAPI ShutdownSocket : std::int32_t {

		RECEIVE,
		SEND,
		BOTH,

	};

}

#endif // _NE_SHUTDOWNSOCKET_H_