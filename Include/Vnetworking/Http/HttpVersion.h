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

		HTTP_1_0 = 1,
		HTTP_1_1 = 2,
		HTTP_2_0 = 3,
		HTTP_3_0 = 4,

	};

	VNETHTTPAPI std::string ToString(const HttpVersion version);

}

#endif // _NE_HTTP_HTTPVERSION_H_