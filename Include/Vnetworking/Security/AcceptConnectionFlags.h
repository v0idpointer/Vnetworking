/*
	Vnetworking SSL/TLS Library
	Copyright (C) V0idPointer
*/

#ifndef _NE_SECURITY_ACCEPTCONNECTIONFLAGS_H_
#define _NE_SECURITY_ACCEPTCONNECTIONFLAGS_H_

#include <Vnetworking/Exports.h>

#include <cstdint>

namespace Vnetworking::Security {

	enum class VNETSECURITYAPI AcceptConnectionFlags : std::uint32_t {

		NONE = 0,
		MUTUAL_AUTHENTICATION = 1,

	};

	static inline AcceptConnectionFlags operator| (const AcceptConnectionFlags a, const AcceptConnectionFlags b) noexcept {
		return static_cast<AcceptConnectionFlags>(static_cast<std::int32_t>(a) | static_cast<std::int32_t>(b));
	}

	static inline AcceptConnectionFlags& operator|= (AcceptConnectionFlags& a, const AcceptConnectionFlags b) noexcept {
		a = (a | b);
		return a;
	}

	static inline AcceptConnectionFlags operator& (const AcceptConnectionFlags a, const AcceptConnectionFlags b) noexcept {
		return static_cast<AcceptConnectionFlags>(static_cast<std::int32_t>(a) & static_cast<std::int32_t>(b));
	}

	static inline AcceptConnectionFlags& operator&= (AcceptConnectionFlags& a, const AcceptConnectionFlags b) noexcept {
		a = (a & b);
		return a;
	}

	static inline AcceptConnectionFlags operator~ (const AcceptConnectionFlags a) noexcept {
		return static_cast<AcceptConnectionFlags>(~static_cast<std::int32_t>(a));
	}

}

#endif // _NE_SECURITY_ACCEPTCONNECTIONFLAGS_H_