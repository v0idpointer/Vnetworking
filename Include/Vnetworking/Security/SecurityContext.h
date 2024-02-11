/*
	Vnetworking SSL/TLS Library
	Copyright (C) V0idPointer
*/

#ifndef _NE_SECURITY_SECURITYCONTEXT_H_
#define _NE_SECURITY_SECURITYCONTEXT_H_

#include <Vnetworking/Exports.h>
#include <Vnetworking/Security/ApplicationRole.h>
#include <Vnetworking/Security/SecurityProtocol.h>
#include <Vnetworking/Security/Certificates/Certificate.h>
#include <Vnetworking/Security/SecureConnection.h>
#include <Vnetworking/Security/SecurityException.h>

#include <optional>

namespace Vnetworking::Security {

	class VNETSECURITYAPI SecurityContext {

	private:
		std::optional<Certificates::Certificate> m_cert;
		SecurityProtocol m_protocol;
		ApplicationRole m_role;

		NativeSecurityContext_t m_credentialsHandle;

	public:
		SecurityContext(const std::optional<Certificates::Certificate>& cert, const SecurityProtocol protocol, const ApplicationRole role);
		SecurityContext(const SecurityContext&) = delete;
		SecurityContext(SecurityContext&& ctx) noexcept;
		virtual ~SecurityContext(void);

		SecurityContext& operator= (const SecurityContext&) = delete;
		SecurityContext& operator= (SecurityContext&& ctx) noexcept;
		bool operator== (const SecurityContext& ctx) const;

		NativeSecurityContext_t GetNativeSecurityContext(void) const;

		const std::optional<Certificates::Certificate>& GetCertificate(void) const;
		SecurityProtocol GetSecurityProtocol(void) const;
		ApplicationRole GetApplicationRole(void) const;

		std::optional<SecureConnection> AcceptConnection(const Sockets::NativeSocket_t socket, SecurityException& ex) const;
		std::optional<SecureConnection> AcceptConnection(const Sockets::NativeSocket_t socket) const;
		// TODO: std::optional<SecureConnection> InitializeConnection(const Sockets::NativeSocket_t socket);

	};

}

#endif // _NE_SECURITY_SECURITYCONTEXT_H_