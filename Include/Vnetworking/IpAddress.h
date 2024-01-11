/*
	Vnetworking Library
	Copyright (C) V0idPointer
*/

#ifndef _NE_IPADDRESS_H_
#define _NE_IPADDRESS_H_

#include <Vnetworking/Exports.h>

#include <string>
#include <cstdint>
#include <array>

namespace Vnetworking {

	typedef std::uint16_t Port;

	class VNETCOREAPI IpAddress {

	private:
		bool m_isVersion6;
		std::array<std::uint8_t, 16> m_bytes;

	public:
		IpAddress(void);
		IpAddress(const std::uint8_t aa, const std::uint8_t bb, const std::uint8_t cc, const std::uint8_t dd);
		IpAddress(const std::array<std::uint8_t, 4>& address);
		IpAddress(const std::array<std::uint8_t, 16>& address);
		IpAddress(const IpAddress& address);
		IpAddress(IpAddress&& address) noexcept;
		virtual ~IpAddress(void);

		IpAddress& operator= (const IpAddress& address);
		IpAddress& operator= (IpAddress&& address) noexcept;
		bool operator== (const IpAddress& address) const;

		bool IsVersion6(void) const;
		std::string ToString(void) const;
		const std::array<std::uint8_t, 16>& GetAddressBytes(void) const;

		bool IsPublicAddress(void) const;
		bool IsPrivateAddress(void) const;

		static inline IpAddress Any(void) noexcept { return { 0, 0, 0, 0, }; };
		static inline IpAddress Localhost(void) noexcept { return { 127, 0, 0, 1, }; };
		static inline IpAddress Broadcast(void) noexcept { return { 255, 255, 255, 255, }; };

		static inline IpAddress Any6(void) noexcept { return { { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, } }; };
		static inline IpAddress Localhost6(void) noexcept { return { { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, } }; };

		static IpAddress Parse(const std::string& ipAddress);

	};

}

#endif // _NE_IPADDRESS_H_