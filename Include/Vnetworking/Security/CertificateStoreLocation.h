/*
	Vnetworking SSL/TLS Library
	Copyright (C) V0idPointer
*/

#ifndef _NE_SECURITY_CERTIFICATESTORELOCATION_H_
#define _NE_SECURITY_CERTIFICATESTORELOCATION_H_

#include <Vnetworking/Exports.h>

namespace Vnetworking::Security {

	enum class VNETSECURITYAPI CertificateStoreLocation : std::uint32_t {

		CURRENT_USER,
		LOCAL_MACHINE,

	};

}

#endif // _NE_SECURITY_CERTIFICATESTORELOCATION_H_