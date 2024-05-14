/*
	Vnetworking Library
	Copyright (C) V0idPointer
*/

#ifndef _NE_SOCKETFLAGS_H_
#define _NE_SOCKETFLAGS_H_

#include <Vnetworking/Exports.h>

#include <cstdint>

namespace Vnetworking::Sockets {

	enum class VNETCOREAPI SocketFlags : std::int32_t { 
	
		NONE = 0,
		OUT_OF_BAND = 1,
		PEEK = 2,
		DONT_ROUTE = 4,

	};

	static inline SocketFlags operator| (const SocketFlags a, const SocketFlags b) noexcept {
		return static_cast<SocketFlags>(static_cast<std::int32_t>(a) | static_cast<std::int32_t>(b));
	}

	static inline SocketFlags& operator|= (SocketFlags& a, const SocketFlags b) noexcept {
		a = (a | b);
		return a;
	}

	static inline SocketFlags operator& (const SocketFlags a, const SocketFlags b) noexcept {
		return static_cast<SocketFlags>(static_cast<std::int32_t>(a) & static_cast<std::int32_t>(b));
	}

	static inline SocketFlags& operator&= (SocketFlags& a, const SocketFlags b) noexcept {
		a = (a & b);
		return a;
	}

	static inline SocketFlags operator~ (const SocketFlags a) noexcept {
		return static_cast<SocketFlags>(~static_cast<std::int32_t>(a));
	}

}

#endif // _NE_SOCKETFLAGS_H_