/*
	Vnetworking Library
	Copyright (C) V0idPointer
*/

#ifndef _NE_DNS_H_
#define _NE_DNS_H_

#include <Vnetworking/Exports.h>
#include <Vnetworking/IpAddress.h>
#include <Vnetworking/Dns/DnsLookupResult.h>

#include <string>

namespace Vnetworking::Dns {

	class VNETCOREAPI DNS final { 
	
	public:
		DNS(void) = delete;

		static DnsLookupResult Resolve(const std::string& hostname);
		static DnsLookupResult Resolve(const IpAddress& ipAddress);

	};

}

#endif // _NE_DNS_H_