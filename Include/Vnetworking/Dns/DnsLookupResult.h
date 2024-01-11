/*
	Vnetworking Library
	Copyright (C) V0idPointer
*/

#ifndef _NE_DNSLOOKUPRESULT_H_
#define _NE_DNSLOOKUPRESULT_H_

#include <Vnetworking/Exports.h>
#include <Vnetworking/IpAddress.h>

#include <string>
#include <vector>

namespace Vnetworking::Dns {

	class VNETCOREAPI DnsLookupResult { 
	
	private:
		std::string m_canonicalName;
		std::vector<std::string> m_aliases;
		std::vector<IpAddress> m_addresses;

	public:
		DnsLookupResult(void);
		DnsLookupResult(const std::string&, const std::vector<std::string>&, const std::vector<IpAddress>&);
		DnsLookupResult(const DnsLookupResult& other);
		DnsLookupResult(DnsLookupResult&& other) noexcept;
		virtual ~DnsLookupResult(void);

		DnsLookupResult& operator= (const DnsLookupResult& other);
		DnsLookupResult& operator= (DnsLookupResult&& other) noexcept;
		bool operator== (const DnsLookupResult& other) const;

		std::string GetCanonicalName(void) const;
		const std::vector<std::string>& GetAliases(void) const;
		const std::vector<IpAddress>& GetAddresses(void) const;
		std::vector<std::string>& GetAliases(void);
		std::vector<IpAddress>& GetAddresses(void);

		void SetCanonicalName(const std::string& canonicalName);
		void SetAliases(const std::vector<std::string>& aliases);
		void SetAddresses(const std::vector<IpAddress>& addresses);
	
	};

}

#endif // _NE_DNSLOOKUPRESULT_H_