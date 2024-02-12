#include <Vnetworking/Security/SecureConnection.h>
#include <Vnetworking/Security/SecurityException.h>
#include "Schannel.h"

#include <exception>
#include <stdexcept>

using namespace Vnetworking::Security;
using namespace Vnetworking::Security::Private;
using namespace Vnetworking::Security::Certificates;

SecureConnection::SecureConnection(const NativeSecurityContext_t securityContext, const std::nullptr_t) {
	
	if ((securityContext.Lower == 0) && (securityContext.Upper == 0))
		throw std::invalid_argument("Invalid native security context.");

	this->m_securityContext = securityContext;

}

SecureConnection::SecureConnection(SecureConnection&& secureConn) noexcept {
	this->m_securityContext = { 0 };
	this->operator= (std::move(secureConn));
}

SecureConnection::~SecureConnection() {

	SecHandle securityContext = { 0 };
	ToSchannelHandle(this->m_securityContext, securityContext);

	if (IsValidSecHandle(securityContext)) {
		DeleteSecurityContext(&securityContext);
		this->m_securityContext = { 0 };
	}

}

SecureConnection& SecureConnection::operator= (SecureConnection&& secureConn) noexcept {

	SecHandle securityContext = { 0 };
	ToSchannelHandle(this->m_securityContext, securityContext);

	if (IsValidSecHandle(securityContext)) {
		DeleteSecurityContext(&securityContext);
		this->m_securityContext = { 0 };
	}

	this->m_securityContext = std::move(secureConn.m_securityContext);
	secureConn.m_securityContext = { 0 };

	return static_cast<SecureConnection&>(*this);
}

bool SecureConnection::operator== (const SecureConnection& secureConn) const {
	if (this->m_securityContext.Lower != secureConn.m_securityContext.Lower) return false;
	if (this->m_securityContext.Upper != secureConn.m_securityContext.Upper) return false;
	return true;
}

NativeSecurityContext_t SecureConnection::GetNativeSecurityContext() const {
	return this->m_securityContext;
}

static SecPkgContext_StreamSizes GetStreamSizes(SecHandle& securityContext) {

	SecPkgContext_StreamSizes streamSizes = { 0 };
	SECURITY_STATUS status = QueryContextAttributesA(
		&securityContext,
		SECPKG_ATTR_STREAM_SIZES,
		&streamSizes
	);

	if (status != SEC_E_OK)
		throw SecurityException(static_cast<std::int32_t>(status));

	return streamSizes;
}

static std::vector<std::uint8_t> EncryptData(SecHandle& securityContext, const std::uint8_t* const data, const std::uint32_t len) {

	const SecPkgContext_StreamSizes streamSizes = GetStreamSizes(securityContext);
	const std::uint32_t length = (streamSizes.cbHeader + len + streamSizes.cbTrailer);
	if (length >= streamSizes.cbMaximumMessage)
		throw std::length_error("The length of the data buffer exceeds the maximum allowable size for encryption.");

	SecBuffer securityBuffer[4] = { 0 };
	SecBufferDesc securityBufferDesc = { 0 };
	std::vector<std::uint8_t> message(length);

	void* const pHeader = reinterpret_cast<void*>(message.data());
	void* const pData = reinterpret_cast<void*>(message.data() + streamSizes.cbHeader);
	void* const pTrailer = reinterpret_cast<void*>(message.data() + streamSizes.cbHeader + len);

	InitializeSecurityBuffer(securityBuffer[0], SECBUFFER_STREAM_HEADER, streamSizes.cbHeader, pHeader);
	InitializeSecurityBuffer(securityBuffer[1], SECBUFFER_DATA, len, pData);
	InitializeSecurityBuffer(securityBuffer[2], SECBUFFER_STREAM_TRAILER, streamSizes.cbTrailer, pTrailer);
	InitializeSecurityBuffer(securityBuffer[3], SECBUFFER_EMPTY, NULL, NULL);
	InitializeSecurityBufferDesc(securityBufferDesc, SECBUFFER_VERSION, 4, securityBuffer);

	memcpy_s(securityBuffer[1].pvBuffer, len, data, len);

	SECURITY_STATUS status = EncryptMessage(
		&securityContext,
		NULL,
		&securityBufferDesc,
		NULL
	);

	if (status != SEC_E_OK)
		throw SecurityException(static_cast<std::int32_t>(status));

	return message;
}

std::vector<std::uint8_t> SecureConnection::Encrypt(const std::vector<std::uint8_t>& data) const {

	SecHandle securityContext = { 0 };
	ToSchannelHandle(this->m_securityContext, securityContext);

	return EncryptData(securityContext, data.data(), static_cast<std::uint32_t>(data.size()));
}

std::vector<std::uint8_t> SecureConnection::EncryptLargeMessage(const std::vector<std::uint8_t>& data) const {

	SecHandle securityContext = { 0 };
	ToSchannelHandle(this->m_securityContext, securityContext);

	const SecPkgContext_StreamSizes streamSizes = GetStreamSizes(securityContext);
	const std::uint32_t chunkSize = (streamSizes.cbMaximumMessage - (streamSizes.cbHeader + streamSizes.cbTrailer + 1));

	std::vector<std::uint8_t> message = { };
	std::size_t offset = 0;
	for (std::size_t i = 0; i < data.size(); i += chunkSize) {
		
		const std::size_t remaining = (data.size() - offset);
		const std::size_t length = std::min<std::size_t>(remaining, chunkSize);
		const std::uint8_t* const pData = (data.data() + offset);
		
		std::vector<std::uint8_t> encryptedChunk = EncryptData(securityContext, pData, static_cast<std::uint32_t>(length));
		message.insert(message.end(), encryptedChunk.begin(), encryptedChunk.end());
		
		offset += length;

	}

	return message;
}

static std::vector<std::uint8_t> DecryptData(SecHandle& securityContext, const std::uint8_t* const data, const std::uint32_t len) {
	
	const SecPkgContext_StreamSizes streamSizes = GetStreamSizes(securityContext);
	if (len > streamSizes.cbMaximumMessage)
		throw std::length_error("The length of the data buffer exceeds the maximum allowable size for decryption.");

	SecBuffer securityBuffer[4] = { 0 };
	SecBufferDesc securityBufferDesc = { 0 };
	std::vector<std::uint8_t> encrypted(len);

	InitializeSecurityBuffer(securityBuffer[0], SECBUFFER_DATA, static_cast<std::uint32_t>(encrypted.size()), encrypted.data());
	InitializeSecurityBuffer(securityBuffer[1], SECBUFFER_EMPTY, NULL, NULL);
	InitializeSecurityBuffer(securityBuffer[2], SECBUFFER_EMPTY, NULL, NULL);
	InitializeSecurityBuffer(securityBuffer[3], SECBUFFER_EMPTY, NULL, NULL);
	InitializeSecurityBufferDesc(securityBufferDesc, SECBUFFER_VERSION, 4, securityBuffer);

	memcpy_s(encrypted.data(), encrypted.size(), data, len);

	SECURITY_STATUS status = DecryptMessage(
		&securityContext,
		&securityBufferDesc,
		NULL,
		NULL
	);

	if (status != SEC_E_OK)
		throw SecurityException(static_cast<std::int32_t>(status));

	std::vector<std::uint8_t> decrypted(securityBuffer[1].cbBuffer);
	memcpy_s(decrypted.data(), decrypted.size(), securityBuffer[1].pvBuffer, securityBuffer[1].cbBuffer);

	return decrypted;
}

std::vector<std::uint8_t> SecureConnection::Decrypt(const std::vector<std::uint8_t>& data) const {

	SecHandle securityContext = { 0 };
	ToSchannelHandle(this->m_securityContext, securityContext);
	
	return DecryptData(securityContext, data.data(), static_cast<std::uint32_t>(data.size()));
}

std::optional<Certificate> SecureConnection::GetCertificate() const {

	SecHandle securityContext = { 0 };
	ToSchannelHandle(this->m_securityContext, securityContext);

	PCCERT_CONTEXT pCertContext = NULL;
	SECURITY_STATUS status = QueryContextAttributesA(
		&securityContext,
		SECPKG_ATTR_LOCAL_CERT_CONTEXT,
		&pCertContext
	);

	if (status != SEC_E_OK) {
		if (status == SEC_E_NO_CREDENTIALS) return std::nullopt;
		else throw SecurityException(static_cast<std::int32_t>(status));
	}

	return Certificate(pCertContext, true, nullptr);
}

std::optional<Certificate> SecureConnection::GetPeerCertificate() const {

	SecHandle securityContext = { 0 };
	ToSchannelHandle(this->m_securityContext, securityContext);

	PCCERT_CONTEXT pCertContext = NULL;
	SECURITY_STATUS status = QueryContextAttributesA(
		&securityContext,
		SECPKG_ATTR_REMOTE_CERT_CONTEXT,
		&pCertContext
	);

	if (status != SEC_E_OK) {
		if (status == SEC_E_NO_CREDENTIALS) return std::nullopt;
		else throw SecurityException(static_cast<std::int32_t>(status));
	}

	return Certificate(pCertContext, true, nullptr);
}