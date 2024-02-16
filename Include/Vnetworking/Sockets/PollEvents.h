/*
	Vnetworking Library
	Copyright (C) V0idPointer
*/

#ifndef _NE_POLLEVENTS_H_
#define _NE_POLLEVENTS_H_

#include <Vnetworking/Exports.h>

#include <cstdint>

namespace Vnetworking::Sockets {

	enum class VNETCOREAPI PollEvents : std::uint32_t {

		READ = 1,
		WRITE = 2,
		ERROR = 3,

	};

}

#endif // _NE_POLLEVENTS_H_