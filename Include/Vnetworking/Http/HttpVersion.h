/*
	Vnetworking HTTP Library
	Copyright (C) V0idPointer
*/

#ifndef _NE_HTTP_HTTPVERSION_H_
#define _NE_HTTP_HTTPVERSION_H_

#include <Vnetworking/Exports.h>

#include <cstdint>
#include <string>

namespace Vnetworking::Http {

	enum class VNETHTTPAPI HttpVersion : std::uint16_t {

		HTTP_0_9 = 1,
		HTTP_1_0 = 2,
		HTTP_1_1 = 3,
		HTTP_2_0 = 4,
		HTTP_3_0 = 5,

	};

	VNETHTTPAPI std::string ToString(const HttpVersion version);

}

#endif // _NE_HTTP_HTTPVERSION_H_