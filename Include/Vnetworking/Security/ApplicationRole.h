/*
	Vnetworking SSL/TLS Library
	Copyright (C) V0idPointer
*/

#ifndef _NE_SECURITY_APPLICATIONROLE_H_
#define _NE_SECURITY_APPLICATIONROLE_H_

#include <Vnetworking/Exports.h>

#include <cstdint>

namespace Vnetworking::Security {

	enum class VNETSECURITYAPI ApplicationRole : std::uint32_t {

		SERVER = 1,
		CLIENT = 2,

	};

}

#endif // _NE_SECURITY_APPLICATIONROLE_H_