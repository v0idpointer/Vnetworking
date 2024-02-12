/*
	Vnetworking SSL/TLS Library
	Copyright (C) V0idPointer
*/

#ifndef _NE_SECURITY_CERTIFICATESTORE_H_
#define _NE_SECURITY_CERTIFICATESTORE_H_

#include <Vnetworking/Exports.h>
#include <Vnetworking/Security/Certificates/Certificate.h>
#include <Vnetworking/Security/Certificates/CertificateStoreName.h>
#include <Vnetworking/Security/Certificates/CertificateStoreLocation.h>

#include <optional>
#include <functional>

namespace Vnetworking::Security::Certificates {

	typedef void* NativeCertificateStoreHandle_t;

	class VNETSECURITYAPI CertificateStore {

	private:
		NativeCertificateStoreHandle_t m_hCertStore;

	public:
		CertificateStore(const std::string& storeName);
		CertificateStore(const CertificateStoreName storeName);
		CertificateStore(const std::string& storeName, const CertificateStoreLocation storeLocation);
		CertificateStore(const CertificateStoreName storeName, const CertificateStoreLocation storeLocation);
		CertificateStore(const NativeCertificateStoreHandle_t hCertStore, const std::nullptr_t);
		CertificateStore(const CertificateStore&) = delete;
		CertificateStore(CertificateStore&& certStore) noexcept;
		virtual ~CertificateStore(void);

		CertificateStore& operator= (const CertificateStore&) = delete;
		CertificateStore& operator= (CertificateStore&& certStore) noexcept;
		bool operator== (const CertificateStore& certStore) const;

		NativeCertificateStoreHandle_t GetNativeCertificateStoreHandle(void) const;

		void AddCertificate(const Certificate& cert);
		void DeleteCertificate(const Certificate& cert);

		std::int32_t GetCertificateCount(void) const;
		std::vector<Certificate> GetCertificates(void) const;
		std::optional<Certificate> EnumCertificates(const std::function<bool(const Certificate&)> fn) const;

	};

}

#endif // _NE_SECURITY_CERTIFICATESTORE_H_