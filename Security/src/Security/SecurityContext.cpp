#include <Vnetworking/Security/SecurityContext.h>
#include "Schannel.h"

#include <map>
#include <exception>
#include <stdexcept>

using namespace Vnetworking::Security;
using namespace Vnetworking::Security::Private;
using namespace Vnetworking::Security::Certificates;
using namespace Vnetworking::Sockets;

static std::map<std::pair<SecurityProtocol, ApplicationRole>, DWORD> s_configurations = { 

	{ { SecurityProtocol::SSL_2_0, ApplicationRole::SERVER }, SP_PROT_SSL2_SERVER },
	{ { SecurityProtocol::SSL_2_0, ApplicationRole::CLIENT }, SP_PROT_SSL2_CLIENT },
	{ { SecurityProtocol::SSL_3_0, ApplicationRole::SERVER }, SP_PROT_SSL3_SERVER },
	{ { SecurityProtocol::SSL_3_0, ApplicationRole::CLIENT }, SP_PROT_SSL3_CLIENT },
	{ { SecurityProtocol::TLS_1_0, ApplicationRole::SERVER }, SP_PROT_TLS1_0_SERVER },
	{ { SecurityProtocol::TLS_1_0, ApplicationRole::CLIENT }, SP_PROT_TLS1_0_CLIENT },
	{ { SecurityProtocol::TLS_1_1, ApplicationRole::SERVER }, SP_PROT_TLS1_1_SERVER },
	{ { SecurityProtocol::TLS_1_1, ApplicationRole::CLIENT }, SP_PROT_TLS1_1_CLIENT },
	{ { SecurityProtocol::TLS_1_2, ApplicationRole::SERVER }, SP_PROT_TLS1_2_SERVER },
	{ { SecurityProtocol::TLS_1_2, ApplicationRole::CLIENT }, SP_PROT_TLS1_2_CLIENT },
	{ { SecurityProtocol::TLS_1_3, ApplicationRole::SERVER }, SP_PROT_TLS1_3_SERVER },
	{ { SecurityProtocol::TLS_1_3, ApplicationRole::CLIENT }, SP_PROT_TLS1_3_CLIENT },

};

SecurityContext::SecurityContext(const std::optional<Certificate>& cert, const SecurityProtocol protocol, const ApplicationRole role) {
	
	this->m_credentialsHandle = { 0 };
	this->m_cert = cert;
	this->m_protocol = protocol;
	this->m_role = role;

	std::uint32_t mask = 1;
	std::uint32_t enabledProtocols = 0;
	const std::uint32_t protocols = static_cast<std::uint32_t>(protocol);

	while (mask) {

		if (protocols & mask) {

			SecurityProtocol prot = static_cast<SecurityProtocol>(mask);
			if (!s_configurations.contains({ prot, this->m_role }))
				throw std::invalid_argument("Invalid security protocol / application role configuration.");

			enabledProtocols |= s_configurations.at({ prot, this->m_role });

		}

		mask <<= 1;
		if (mask >= 0xFFFF) break;

	}

	PCCERT_CONTEXT pCertContext = NULL;
	if (this->m_cert.has_value())
		pCertContext = reinterpret_cast<PCCERT_CONTEXT>(this->m_cert->GetNativeCertificateContext());

	CredHandle credentialsHandle = { 0 };
	SECURITY_STATUS status = NULL;
	SCHANNEL_CRED cred = { 0 };
	TimeStamp ts;

	cred.dwVersion = SCHANNEL_CRED_VERSION;
	cred.grbitEnabledProtocols = static_cast<DWORD>(enabledProtocols);
	cred.paCred = &pCertContext;
	cred.cCreds = (this->m_cert.has_value() ? 1 : 0);

	status = AcquireCredentialsHandleA(
		NULL,
		const_cast<LPSTR>(UNISP_NAME_A),
		((this->m_role == ApplicationRole::SERVER) ? SECPKG_CRED_INBOUND : SECPKG_CRED_OUTBOUND),
		NULL,
		&cred,
		NULL,
		NULL,
		&credentialsHandle,
		&ts
	);

	if (status != SEC_E_OK)
		throw SecurityException(static_cast<std::int32_t>(GetLastError()));

	ToVnetsecHandle(credentialsHandle, this->m_credentialsHandle);

}

SecurityContext::SecurityContext(SecurityContext&& ctx) noexcept {
	this->m_credentialsHandle = { 0 };
	this->operator= (std::move(ctx));
}

SecurityContext::~SecurityContext() {

	CredHandle credentialsHandle = { 0 };
	ToSchannelHandle(this->m_credentialsHandle, credentialsHandle);

	if (IsValidSecHandle(credentialsHandle)) {
		FreeCredentialHandle(&credentialsHandle);
		this->m_credentialsHandle = { 0 };
	}

}

SecurityContext& SecurityContext::operator= (SecurityContext&& ctx) noexcept {
	
	CredHandle credentialsHandle = { 0 };
	ToSchannelHandle(this->m_credentialsHandle, credentialsHandle);

	if (IsValidSecHandle(credentialsHandle)) {
		FreeCredentialHandle(&credentialsHandle);
		this->m_credentialsHandle = { 0 };
	}

	this->m_credentialsHandle = std::move(ctx.m_credentialsHandle);
	this->m_cert = std::move(ctx.m_cert);
	this->m_protocol = ctx.m_protocol;
	this->m_role = ctx.m_role;

	ctx.m_credentialsHandle = { 0 };
	ctx.m_cert = std::nullopt;

	return static_cast<SecurityContext&>(*this);
}

bool SecurityContext::operator== (const SecurityContext& ctx) const {
	
	if (this->m_cert != ctx.m_cert) return false;
	if (this->m_credentialsHandle.Lower != ctx.m_credentialsHandle.Lower) return false;
	if (this->m_credentialsHandle.Upper != ctx.m_credentialsHandle.Upper) return false;
	if (this->m_protocol != ctx.m_protocol) return false;
	if (this->m_role != ctx.m_role) return false;

	return true;
}

NativeSecurityContext_t SecurityContext::GetNativeSecurityContext() const {
	return this->m_credentialsHandle;
}

const std::optional<Certificate>& SecurityContext::GetCertificate() const {
	return this->m_cert;
}

SecurityProtocol SecurityContext::GetSecurityProtocol() const {
	return this->m_protocol;
}

ApplicationRole SecurityContext::GetApplicationRole() const {
	return this->m_role;
}

std::optional<SecureConnection> SecurityContext::AcceptConnection(const NativeSocket_t socket, SecurityException& ex) const {

	if (this->m_role != ApplicationRole::SERVER)
		throw std::runtime_error("A new connection can only be accepted by a server application.");

	CredHandle credentialsHandle = { 0 };
	ToSchannelHandle(this->m_credentialsHandle, credentialsHandle);

	SecHandle securityContext = { 0 };

	// input buffer:
	std::vector<std::uint8_t> inputBuffer;
	SecBuffer inputSecurityBuffer[2] = { 0 };
	SecBufferDesc inputSecurityBufferDesc = { 0 };

	inputBuffer.resize(HANDSHAKE_BUFFER_LEN);
	InitializeSecurityBuffer(inputSecurityBuffer[0], SECBUFFER_TOKEN, HANDSHAKE_BUFFER_LEN, reinterpret_cast<void*>(inputBuffer.data()));
	InitializeSecurityBuffer(inputSecurityBuffer[1], SECBUFFER_EMPTY, NULL, NULL);
	InitializeSecurityBufferDesc(inputSecurityBufferDesc, SECBUFFER_VERSION, 2, inputSecurityBuffer);

	if (inputSecurityBuffer[0].pvBuffer == nullptr) {
		ex = SecurityException(ERROR_OUTOFMEMORY);
		return std::nullopt;
	}

	// output buffer:
	SecBuffer outputSecurityBuffer[3] = { 0 };
	SecBufferDesc outputSecurityBufferDesc = { 0 };

	InitializeSecurityBuffer(outputSecurityBuffer[0], SECBUFFER_TOKEN, NULL, NULL);
	InitializeSecurityBuffer(outputSecurityBuffer[1], SECBUFFER_ALERT, NULL, NULL);
	InitializeSecurityBuffer(outputSecurityBuffer[2], SECBUFFER_EMPTY, NULL, NULL);
	InitializeSecurityBufferDesc(outputSecurityBufferDesc, SECBUFFER_VERSION, 3, outputSecurityBuffer);

	// handshake:
	DWORD contextRequirements = (ASC_REQ_ALLOCATE_MEMORY | ASC_REQ_CONFIDENTIALITY);
	ULONG contextAttributes = 0;
	TimeStamp ts = { 0 };

	bool authenticationCompleted = false;
	bool firstTime = true;

	while (!authenticationCompleted) {

		inputSecurityBuffer[0].cbBuffer = recv(socket, reinterpret_cast<char*>(inputSecurityBuffer[0].pvBuffer), HANDSHAKE_BUFFER_LEN, NULL);

		SECURITY_STATUS status = AcceptSecurityContext(
			&credentialsHandle,
			(firstTime ? NULL : &securityContext),
			&inputSecurityBufferDesc,
			contextRequirements,
			NULL,
			&securityContext,
			&outputSecurityBufferDesc,
			&contextAttributes,
			&ts
		);

		if (firstTime) firstTime = false;

		switch (status) {

		case SEC_E_OK:
		case SEC_I_CONTINUE_NEEDED:

			if (outputSecurityBuffer[0].cbBuffer > 0)
				send(socket, reinterpret_cast<const char*>(outputSecurityBuffer[0].pvBuffer), outputSecurityBuffer[0].cbBuffer, NULL);

			if (status == SEC_E_OK)
				authenticationCompleted = true;

			break;

		case SEC_I_COMPLETE_AND_CONTINUE:
		case SEC_I_COMPLETE_NEEDED:
			{

				SECURITY_STATUS s = CompleteAuthToken(&securityContext, &outputSecurityBufferDesc);
				if (s != SEC_E_OK) {
					SafeFreeBuffers(inputSecurityBufferDesc, outputSecurityBufferDesc, inputBuffer);
					ex = SecurityException(static_cast<int>(status));
					return std::nullopt;
				}

				if (outputSecurityBuffer[0].cbBuffer > 0)
					send(socket, reinterpret_cast<const char*>(outputSecurityBuffer[0].pvBuffer), outputSecurityBuffer[0].cbBuffer, NULL);

				if (status == SEC_I_COMPLETE_NEEDED)
					authenticationCompleted = true;

			}
			break;

		default:
			SafeFreeBuffers(inputSecurityBufferDesc, outputSecurityBufferDesc, inputBuffer);
			ex = SecurityException(static_cast<int>(status));
			return std::nullopt;
			break;

		}

	}

	SafeFreeBuffers(inputSecurityBufferDesc, outputSecurityBufferDesc, inputBuffer);
	ex = SecurityException(ERROR_SUCCESS);

	NativeSecurityContext_t nativeSecurityContext = { 0 };
	ToVnetsecHandle(securityContext, nativeSecurityContext);

	SecureConnection secureConn = { nativeSecurityContext, nullptr };

	return secureConn;
}

std::optional<SecureConnection> SecurityContext::AcceptConnection(const NativeSocket_t socket) const {
	SecurityException ex = { 0 };
	return this->AcceptConnection(socket, ex);
}