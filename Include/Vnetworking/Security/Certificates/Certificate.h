/*
	Vnetworking SSL/TLS Library
	Copyright (C) V0idPointer
*/

#ifndef _NE_SECURITY_CERTIFICATE_H_
#define _NE_SECURITY_CERTIFICATE_H_

#include <Vnetworking/Exports.h>

#include <string>
#include <vector>
#include <cstdint>
#include <cstddef>
#include <filesystem>
#include <chrono>
#include <utility>

namespace Vnetworking::Security::Certificates {

	typedef const void* NativeCertificateContext_t;

	class VNETSECURITYAPI Certificate {

	private:
		NativeCertificateContext_t m_certificateContext;

	public:
		Certificate(const std::vector<std::uint8_t>& data);
		Certificate(const std::filesystem::path& path);
		Certificate(const std::vector<std::uint8_t>& data, const std::string& password);
		Certificate(const std::filesystem::path& path, const std::string& password);
		Certificate(const NativeCertificateContext_t certificateContext, const std::nullptr_t);
		Certificate(const NativeCertificateContext_t certificateContext, const bool takeOwnership, const std::nullptr_t);
		Certificate(const Certificate& cert);
		Certificate(Certificate&& cert) noexcept;
		virtual ~Certificate(void);

		Certificate& operator= (const Certificate& cert);
		Certificate& operator= (Certificate&& cert) noexcept;
		bool operator== (const Certificate& cert) const;

		NativeCertificateContext_t GetNativeCertificateContext(void) const;

		std::int32_t GetVersion(void) const;
		std::string GetSubject(void) const;
		std::string GetIssuer(void) const;
		std::vector<std::uint8_t> GetSerialNumber(void) const;
		std::chrono::time_point<std::chrono::system_clock> GetNotBefore(void) const;
		std::chrono::time_point<std::chrono::system_clock> GetNotAfter(void) const;
		
		std::pair<std::string, std::string> GetSignatureAlgorithm(void) const;
		std::pair<std::string, std::string> GetPublicKeyAlgorithm(void) const;
		std::vector<std::uint8_t> GetPublicKey(void) const;
		std::vector<std::uint8_t> GetPublicKeyParams(void) const;

	};

}

#endif // _NE_SECURITY_CERTIFICATE_H_