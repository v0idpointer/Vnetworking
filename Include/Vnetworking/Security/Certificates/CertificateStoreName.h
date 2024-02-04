/*
	Vnetworking SSL/TLS Library
	Copyright (C) V0idPointer
*/

#ifndef _NE_SECURITY_CERTIFICATESTORENAME_H_
#define _NE_SECURITY_CERTIFICATESTORENAME_H_

#include <Vnetworking/Exports.h>

#include <cstdint>

namespace Vnetworking::Security::Certificates {

	enum class VNETSECURITYAPI CertificateStoreName : std::uint32_t {

		// Personal
		MY,

		// Intermediate Certification Authorities
		CA,

		// Trusted Root Certification Authorities
		ROOT,

		// Third-Party Root Certification Authorities
		TRUST,

	};

}

#endif // _NE_SECURITY_CERTIFICATESTORENAME_H_