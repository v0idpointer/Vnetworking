/*
	Vnetworking HTTP Library
	Copyright (C) V0idPointer
*/

#ifndef _NE_HTTP_HTTPMETHOD_H_
#define _NE_HTTP_HTTPMETHOD_H_

#include <Vnetworking/Exports.h>

#include <cstdint>
#include <string>

namespace Vnetworking::Http {

	enum class VNETHTTPAPI HttpMethod : std::uint16_t {

		GET = 1,
		HEAD = 2,
		POST = 3,
		PUT = 4,
		DELETE = 5,
		CONNECT = 6,
		OPTIONS = 7,
		TRACE = 8,
		PATCH = 9,

	};

	VNETHTTPAPI std::string ToString(const HttpMethod method);
	VNETHTTPAPI HttpMethod ToMethod(const std::string& method);

	VNETHTTPAPI HttpMethod RegisterHttpMethod(const std::string& text);
	VNETHTTPAPI void UnregisterHttpMethod(const HttpMethod method);

}

#endif // _NE_HTTP_HTTPMETHOD_H_