/*
	Vnetworking SSL/TLS Library
	Copyright (C) V0idPointer
*/

#ifndef _NE_SECURITY_SECURITYPROTOCOL_H_
#define _NE_SECURITY_SECURITYPROTOCOL_H_

#include <Vnetworking/Exports.h>

#include <cstdint>

namespace Vnetworking::Security {

	enum class VNETSECURITYAPI SecurityProtocol : std::uint32_t {

		SSL_2_0 = 1,
		SSL_3_0 = 2,
		TLS_1_0 = 4,
		TLS_1_1 = 8,
		TLS_1_2 = 16,
		TLS_1_3 = 32,

	};

	static inline SecurityProtocol operator| (const SecurityProtocol a, const SecurityProtocol b) noexcept {
		return static_cast<SecurityProtocol>(static_cast<std::int32_t>(a) | static_cast<std::int32_t>(b));
	}

	static inline SecurityProtocol operator|= (const SecurityProtocol a, const SecurityProtocol b) noexcept {
		return (a | b);
	}

	static inline SecurityProtocol operator& (const SecurityProtocol a, const SecurityProtocol b) noexcept {
		return static_cast<SecurityProtocol>(static_cast<std::int32_t>(a) & static_cast<std::int32_t>(b));
	}

	static inline SecurityProtocol operator&= (const SecurityProtocol a, const SecurityProtocol b) noexcept {
		return (a & b);
	}

	static inline SecurityProtocol operator~ (const SecurityProtocol a) noexcept {
		return static_cast<SecurityProtocol>(~static_cast<std::int32_t>(a));
	}

}

#endif // _NE_SECURITY_SECURITYPROTOCOL_H_