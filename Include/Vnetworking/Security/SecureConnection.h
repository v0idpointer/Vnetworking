/*
	Vnetworking SSL/TLS Library
	Copyright (C) V0idPointer
*/

#ifndef _NE_SECURITY_SECURECONNECTION_H_
#define _NE_SECURITY_SECURECONNECTION_H_

#include <Vnetworking/Exports.h>
#include <Vnetworking/Security/Certificates/Certificate.h>

#include <cstdint>
#include <cstddef>
#include <vector>

namespace Vnetworking::Security {

	typedef struct {
		std::uintptr_t Lower;
		std::uintptr_t Upper;
	} NativeSecurityContext_t;

	class VNETSECURITYAPI SecureConnection {

	private:
		NativeSecurityContext_t m_securityContext;

	public:
		SecureConnection(const NativeSecurityContext_t securityContext, const std::nullptr_t);
		SecureConnection(const SecureConnection&) = delete;
		SecureConnection(SecureConnection&& secureConn) noexcept;
		virtual ~SecureConnection(void);

		SecureConnection& operator= (const SecureConnection&) = delete;
		SecureConnection& operator= (SecureConnection&& secureConn) noexcept;
		bool operator== (const SecureConnection& secureConn) const;

		NativeSecurityContext_t GetNativeSecurityContext(void) const;

		std::vector<std::uint8_t> Encrypt(const std::vector<std::uint8_t>& data) const;
		std::vector<std::uint8_t> EncryptLargeMessage(const std::vector<std::uint8_t>& data) const;
		std::vector<std::uint8_t> Decrypt(const std::vector<std::uint8_t>& data) const;
		// TODO: std::vector<std::uint8_t> DecryptLargeMessage(const std::vector<std::uint8_t>& data) const;

		Certificates::Certificate GetCertificate(void) const;
		Certificates::Certificate GetPeerCertificate(void) const;

	};

}

#endif // _NE_SECURITY_SECURECONNECTION_H_