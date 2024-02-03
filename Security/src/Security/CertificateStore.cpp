#include <Vnetworking/Security/CertificateStore.h>
#include <Vnetworking/Security/SecurityException.h>

#include <unordered_map>
#include <stdexcept>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <wincrypt.h>

#pragma comment (lib, "crypt32.lib")

using namespace Vnetworking::Security;

static const std::unordered_map<CertificateStoreLocation, DWORD> s_storeLocations = {

	{ CertificateStoreLocation::CURRENT_USER, CERT_SYSTEM_STORE_CURRENT_USER },
	{ CertificateStoreLocation::LOCAL_MACHINE, CERT_SYSTEM_STORE_LOCAL_MACHINE },

};

static const std::unordered_map<CertificateStoreName, std::string> s_storeNames = { 
	
	{ CertificateStoreName::MY, "MY" },
	{ CertificateStoreName::CA, "CA" },
	{ CertificateStoreName::ROOT, "ROOT" },
	{ CertificateStoreName::TRUST, "Trust" },

};

static inline std::string ToString(const CertificateStoreName storeName) {

	if (!s_storeNames.contains(storeName))
		throw std::invalid_argument("Invalid certificate store name.");

	return s_storeNames.at(storeName);
}

CertificateStore::CertificateStore(const std::string& storeName)
	: CertificateStore(storeName, CertificateStoreLocation::CURRENT_USER) { }

CertificateStore::CertificateStore(const CertificateStoreName storeName)
	: CertificateStore(ToString(storeName), CertificateStoreLocation::CURRENT_USER) { }

CertificateStore::CertificateStore(const std::string& storeName, const CertificateStoreLocation storeLocation) {

	if (!s_storeLocations.contains(storeLocation))
		throw std::invalid_argument("Invalid certificate store location.");

	int nameLen = MultiByteToWideChar(CP_UTF8, NULL, storeName.c_str(), -1, NULL, NULL);
	wchar_t* name = new wchar_t[nameLen];
	MultiByteToWideChar(CP_UTF8, NULL, storeName.c_str(), -1, name, nameLen);

	HCERTSTORE hCertStore = CertOpenStore(
		CERT_STORE_PROV_SYSTEM,
		X509_ASN_ENCODING,
		NULL,
		s_storeLocations.at(storeLocation),
		name
	);

	ZeroMemory(name, nameLen);
	delete[] name;
	name = nullptr;

	if (hCertStore == NULL)
		throw SecurityException(GetLastError());

	this->m_hCertStore = reinterpret_cast<NativeCertificateStoreHandle_t>(hCertStore);

}

CertificateStore::CertificateStore(const CertificateStoreName storeName, const CertificateStoreLocation storeLocation)
	: CertificateStore(ToString(storeName), storeLocation) { }

CertificateStore::CertificateStore(const NativeCertificateStoreHandle_t hCertStore, const std::nullptr_t) {
	if (hCertStore == nullptr)
		throw std::invalid_argument("Invalid native certificate store handle.");
	this->m_hCertStore = hCertStore;
}

CertificateStore::CertificateStore(CertificateStore&& certStore) noexcept {
	this->m_hCertStore = nullptr;
	this->operator= (std::move(certStore));
}

CertificateStore::~CertificateStore() {

	if (this->m_hCertStore) {
		HCERTSTORE hCertStore = reinterpret_cast<HCERTSTORE>(this->m_hCertStore);
		CertCloseStore(hCertStore, NULL);
	}

	this->m_hCertStore = nullptr;

}

CertificateStore& CertificateStore::operator= (CertificateStore&& certStore) noexcept {

	if (this->m_hCertStore) {
		HCERTSTORE hCertStore = reinterpret_cast<HCERTSTORE>(this->m_hCertStore);
		CertCloseStore(hCertStore, NULL);
	}

	this->m_hCertStore = certStore.m_hCertStore;
	certStore.m_hCertStore = nullptr;

	return static_cast<CertificateStore&>(*this);
}

bool CertificateStore::operator== (const CertificateStore& certStore) const {
	return (this->m_hCertStore == certStore.m_hCertStore);
}

NativeCertificateStoreHandle_t CertificateStore::GetNativeCertificateStoreHandle() const {
	return this->m_hCertStore;
}

void CertificateStore::AddCertificate(const Certificate& cert) {

	HCERTSTORE hCertStore = reinterpret_cast<HCERTSTORE>(this->m_hCertStore);
	PCCERT_CONTEXT pCertContext = reinterpret_cast<PCCERT_CONTEXT>(cert.GetNativeCertificateContext());

	BOOL result = CertAddCertificateContextToStore(
		hCertStore,
		pCertContext,
		CERT_STORE_ADD_NEW,
		NULL
	);

	if (!result)
		throw SecurityException(GetLastError());

}

void CertificateStore::DeleteCertificate(const Certificate& cert) {
	
	PCCERT_CONTEXT pCertContext = reinterpret_cast<PCCERT_CONTEXT>(cert.GetNativeCertificateContext());
	
	// CertDeleteCertificateFromStore calls CertFreeCertificateContext on pCertContext.
	// giving a duplicate context will prevent Certificate& cert from becoming invalid.
	if (!CertDeleteCertificateFromStore(CertDuplicateCertificateContext(pCertContext)))
		throw SecurityException(GetLastError());

}

std::int32_t CertificateStore::GetCertificateCount() const {

	HCERTSTORE hCertStore = reinterpret_cast<HCERTSTORE>(this->m_hCertStore);

	std::int32_t count = 0;
	PCCERT_CONTEXT pCertContext = NULL;
	while (pCertContext = CertEnumCertificatesInStore(hCertStore, pCertContext))
		count++;

	return count;
}

std::optional<Certificate> CertificateStore::EnumCertificates(const std::function<bool(const Certificate&)> fn) const {

	HCERTSTORE hCertStore = reinterpret_cast<HCERTSTORE>(this->m_hCertStore);

	PCCERT_CONTEXT pCertContext = NULL;
	while (pCertContext = CertEnumCertificatesInStore(hCertStore, pCertContext)) {
		Certificate cert = { pCertContext, false, nullptr };
		if (fn(cert)) {
			CertFreeCertificateContext(pCertContext);
			return cert;
		}
	}

	return std::nullopt;
}