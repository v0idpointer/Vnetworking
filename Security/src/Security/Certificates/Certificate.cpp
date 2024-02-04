#include <Vnetworking/Security/Certificates/Certificate.h>
#include <Vnetworking/Security/SecurityException.h>

#include <fstream>
#include <sstream>
#include <optional>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <wincrypt.h>

#pragma comment (lib, "crypt32.lib")

using namespace Vnetworking::Security;
using namespace Vnetworking::Security::Certificates;

static std::vector<std::uint8_t> ReadBinaryFile(const std::filesystem::path& path) {

	const std::uintmax_t filesize = std::filesystem::file_size(path);
	std::vector<std::uint8_t> buf(static_cast<std::size_t>(filesize));

	std::ifstream file(path, std::ios::binary);
	file.exceptions(std::ios::badbit | std::ios::failbit);
	file.seekg(0, std::ios::beg);
	file.read(reinterpret_cast<char*>(buf.data()), buf.size());
	file.close();

	return buf;
}

Certificate::Certificate(const std::vector<std::uint8_t>& data) {

	PCCERT_CONTEXT pCertContext = CertCreateCertificateContext(
		(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING),
		const_cast<LPBYTE>(data.data()),
		static_cast<DWORD>(data.size())
	);

	if (pCertContext == NULL)
		throw SecurityException(GetLastError());

	this->m_certificateContext = reinterpret_cast<NativeCertificateContext_t>(pCertContext);

}

Certificate::Certificate(const std::filesystem::path& path)
	: Certificate(ReadBinaryFile(path)) { }

Certificate::Certificate(const std::vector<std::uint8_t>& data, const std::string& password) {

	CRYPT_DATA_BLOB blob;
	blob.cbData = static_cast<DWORD>(data.size());
	blob.pbData = const_cast<BYTE*>(data.data());

	if (!PFXIsPFXBlob(&blob))
		throw SecurityException(ERROR_INVALID_DATA);

	int pwLen = MultiByteToWideChar(CP_UTF8, NULL, password.c_str(), -1, NULL, NULL);
	wchar_t* pw = new wchar_t[pwLen];
	MultiByteToWideChar(CP_UTF8, NULL, password.c_str(), -1, pw, pwLen);

	if (!PFXVerifyPassword(&blob, pw, NULL))
		throw SecurityException(ERROR_INVALID_PASSWORD);

	HCERTSTORE hCertStore = PFXImportCertStore(&blob, pw, NULL);

	SecureZeroMemory(pw, pwLen);
	delete[] pw;
	pw = nullptr;

	if (hCertStore == NULL)
		throw SecurityException(GetLastError());

	PCCERT_CONTEXT pCertContext = CertEnumCertificatesInStore(hCertStore, NULL);
	if (pCertContext == NULL) {
		DWORD error = GetLastError();
		CertCloseStore(hCertStore, NULL);
		throw SecurityException(error);
	}

	CertCloseStore(hCertStore, NULL);
	this->m_certificateContext = reinterpret_cast<NativeCertificateContext_t>(pCertContext);

}

Certificate::Certificate(const std::filesystem::path& path, const std::string& password)
	: Certificate(ReadBinaryFile(path), password) { }

Certificate::Certificate(const NativeCertificateContext_t certificateContext, const std::nullptr_t)
	: Certificate(certificateContext, true, nullptr) { }

Certificate::Certificate(const NativeCertificateContext_t certificateContext, const bool takeOwnership, const std::nullptr_t) {

	if (certificateContext == nullptr)
		throw std::invalid_argument("Invalid native certificate context.");

	if (takeOwnership) this->m_certificateContext = certificateContext;
	else {
		PCCERT_CONTEXT pCertContext = reinterpret_cast<PCCERT_CONTEXT>(certificateContext);
		PCCERT_CONTEXT pDuplicateContext = CertDuplicateCertificateContext(pCertContext);
		this->m_certificateContext = reinterpret_cast<NativeCertificateContext_t>(pDuplicateContext);
	}

}

Certificate::Certificate(const Certificate& cert) {
	this->m_certificateContext = nullptr;
	this->operator= (cert);
}

Certificate::Certificate(Certificate&& cert) noexcept {
	this->m_certificateContext = nullptr;
	this->operator= (std::move(cert));
}

Certificate::~Certificate() {

	if (this->m_certificateContext) {
		PCCERT_CONTEXT pCertContext = reinterpret_cast<PCCERT_CONTEXT>(this->m_certificateContext);
		CertFreeCertificateContext(pCertContext);
	}

	this->m_certificateContext = nullptr;

}

Certificate& Certificate::operator= (const Certificate& cert) {

	if (this->m_certificateContext) {
		PCCERT_CONTEXT pCertContext = reinterpret_cast<PCCERT_CONTEXT>(this->m_certificateContext);
		CertFreeCertificateContext(pCertContext);
	}

	PCCERT_CONTEXT pCertContext = reinterpret_cast<PCCERT_CONTEXT>(cert.m_certificateContext);
	PCCERT_CONTEXT pDuplicateContext = CertDuplicateCertificateContext(pCertContext);
	this->m_certificateContext = reinterpret_cast<NativeCertificateContext_t>(pDuplicateContext);

	return static_cast<Certificate&>(*this);
}

Certificate& Certificate::operator= (Certificate&& cert) noexcept {

	if (this->m_certificateContext) {
		PCCERT_CONTEXT pCertContext = reinterpret_cast<PCCERT_CONTEXT>(this->m_certificateContext);
		CertFreeCertificateContext(pCertContext);
	}

	this->m_certificateContext = cert.m_certificateContext;
	cert.m_certificateContext = nullptr;
	
	return static_cast<Certificate&>(*this);
}

bool Certificate::operator== (const Certificate& cert) const {
	return (this->m_certificateContext == cert.m_certificateContext);
}

NativeCertificateContext_t Certificate::GetNativeCertificateContext() const {
	return this->m_certificateContext;
}

std::int32_t Certificate::GetVersion() const {
	PCCERT_CONTEXT pCertContext = reinterpret_cast<PCCERT_CONTEXT>(this->m_certificateContext);
	return (pCertContext->pCertInfo->dwVersion + 1);
}

static std::optional<std::string> GetDistinguishedNameAttrib(PCCERT_CONTEXT pCertContext, const char* const attrib, const bool issuerFlag) noexcept {

	DWORD len = CertGetNameStringA(
		pCertContext,
		CERT_NAME_ATTR_TYPE,
		(issuerFlag ? CERT_NAME_ISSUER_FLAG : NULL),
		reinterpret_cast<void*>(const_cast<char*>(attrib)),
		NULL,
		NULL
	);

	if (len <= 1) 
		return std::nullopt;

	std::string str;
	str.resize(static_cast<std::size_t>(len));

	CertGetNameStringA(
		pCertContext,
		CERT_NAME_ATTR_TYPE,
		(issuerFlag ? CERT_NAME_ISSUER_FLAG : NULL),
		reinterpret_cast<void*>(const_cast<char*>(attrib)),
		const_cast<char*>(str.c_str()),
		static_cast<DWORD>(str.length())
	);

	str.resize(str.length() - 1);

	return str;
}

static std::string GetDistinguishedName(PCCERT_CONTEXT pCertContext, const bool issuerFlag) {

	std::ostringstream stream;

	const std::optional<std::string> commonName = GetDistinguishedNameAttrib(pCertContext, szOID_COMMON_NAME, issuerFlag);
	const std::optional<std::string> organizationalUnit = GetDistinguishedNameAttrib(pCertContext, szOID_ORGANIZATIONAL_UNIT_NAME, issuerFlag);
	const std::optional<std::string> organization = GetDistinguishedNameAttrib(pCertContext, szOID_ORGANIZATION_NAME, issuerFlag);
	const std::optional<std::string> locality = GetDistinguishedNameAttrib(pCertContext, szOID_LOCALITY_NAME, issuerFlag);
	const std::optional<std::string> state = GetDistinguishedNameAttrib(pCertContext, szOID_STATE_OR_PROVINCE_NAME, issuerFlag);
	const std::optional<std::string> country = GetDistinguishedNameAttrib(pCertContext, szOID_COUNTRY_NAME, issuerFlag);

	if (commonName.has_value()) stream << "CN=" << commonName.value();
	if (organizationalUnit.has_value()) stream << (stream.str().length() ? ", " : "") << "OU=" << organizationalUnit.value();
	if (organization.has_value()) stream << (stream.str().length() ? ", " : "") << "O=" << organization.value();
	if (locality.has_value()) stream << (stream.str().length() ? ", " : "") << "L=" << locality.value();
	if (state.has_value()) stream << (stream.str().length() ? ", " : "") << "ST=" << state.value();
	if (country.has_value()) stream << (stream.str().length() ? ", " : "") << "C=" << country.value();

	return stream.str();
}

std::string Certificate::GetSubject() const {
	PCCERT_CONTEXT pCertContext = reinterpret_cast<PCCERT_CONTEXT>(this->m_certificateContext);
	return GetDistinguishedName(pCertContext, false);
}

std::string Certificate::GetIssuer() const {
	PCCERT_CONTEXT pCertContext = reinterpret_cast<PCCERT_CONTEXT>(this->m_certificateContext);
	return GetDistinguishedName(pCertContext, true);
}

std::vector<std::uint8_t> Certificate::GetSerialNumber() const {

	PCCERT_CONTEXT pCertContext = reinterpret_cast<PCCERT_CONTEXT>(this->m_certificateContext);
	DWORD cbSerialNumber = pCertContext->pCertInfo->SerialNumber.cbData;
	LPBYTE pbSerialNumber = pCertContext->pCertInfo->SerialNumber.pbData;

	std::vector<std::uint8_t> data(static_cast<std::size_t>(cbSerialNumber));
	for (DWORD i = 0; i < cbSerialNumber; ++i)
		data[i] = static_cast<std::uint8_t>(pbSerialNumber[cbSerialNumber - (i + 1)]);

	return data;
}

static std::chrono::time_point<std::chrono::system_clock> FiletimeToTimepoint(const FILETIME& ft) noexcept {

	ULARGE_INTEGER ull;
	ull.LowPart = ft.dwLowDateTime;
	ull.HighPart = ft.dwHighDateTime;

	const std::uint64_t time = (static_cast<std::uint64_t>(ull.QuadPart) - 116444736000000000ULL);
	const std::chrono::nanoseconds ns(time * 100);
	const std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(ns);

	return std::chrono::time_point<std::chrono::system_clock>(ms);
}

std::chrono::time_point<std::chrono::system_clock> Certificate::GetNotBefore() const {

	PCCERT_CONTEXT pCertContext = reinterpret_cast<PCCERT_CONTEXT>(this->m_certificateContext);
	FILETIME ft = pCertContext->pCertInfo->NotBefore;

	return FiletimeToTimepoint(ft);
}

std::chrono::time_point<std::chrono::system_clock> Certificate::GetNotAfter() const {

	PCCERT_CONTEXT pCertContext = reinterpret_cast<PCCERT_CONTEXT>(this->m_certificateContext);
	FILETIME ft = pCertContext->pCertInfo->NotAfter;

	return FiletimeToTimepoint(ft);
}

static std::string GetFriendlyNameFromOid(const std::string& oid) noexcept {

	std::string friendlyName;

	PCCRYPT_OID_INFO info = CryptFindOIDInfo(
		CRYPT_OID_INFO_OID_KEY,
		const_cast<void*>(reinterpret_cast<const void*>(oid.c_str())),
		NULL
	);

	if (info) {
		int len = WideCharToMultiByte(CP_UTF8, NULL, info->pwszName, -1, NULL, NULL, NULL, NULL);
		friendlyName.resize(len);
		WideCharToMultiByte(CP_UTF8, NULL, info->pwszName, -1, const_cast<char*>(friendlyName.c_str()), len, NULL, NULL);
	}

	return friendlyName;
}

std::pair<std::string, std::string> Certificate::GetSignatureAlgorithm() const {

	PCCERT_CONTEXT pCertContext = reinterpret_cast<PCCERT_CONTEXT>(this->m_certificateContext);
	const std::string oid = { pCertContext->pCertInfo->SignatureAlgorithm.pszObjId };

	return { oid, GetFriendlyNameFromOid(oid) };
}

std::pair<std::string, std::string> Certificate::GetPublicKeyAlgorithm() const {

	PCCERT_CONTEXT pCertContext = reinterpret_cast<PCCERT_CONTEXT>(this->m_certificateContext);
	const std::string oid = { pCertContext->pCertInfo->SubjectPublicKeyInfo.Algorithm.pszObjId };

	return { oid, GetFriendlyNameFromOid(oid) };
}

std::vector<std::uint8_t> Certificate::GetPublicKey() const {

	PCCERT_CONTEXT pCertContext = reinterpret_cast<PCCERT_CONTEXT>(this->m_certificateContext);

	DWORD keyLen = pCertContext->pCertInfo->SubjectPublicKeyInfo.PublicKey.cbData;
	LPBYTE key = pCertContext->pCertInfo->SubjectPublicKeyInfo.PublicKey.pbData;

	std::vector<std::uint8_t> data(static_cast<std::size_t>(keyLen));
	for (DWORD i = 0; i < keyLen; ++i) 
		data[i] = static_cast<std::uint8_t>(key[i]);

	return data;
}

std::vector<std::uint8_t> Certificate::GetPublicKeyParams() const {

	PCCERT_CONTEXT pCertContext = reinterpret_cast<PCCERT_CONTEXT>(this->m_certificateContext);

	DWORD paramsLen = pCertContext->pCertInfo->SubjectPublicKeyInfo.Algorithm.Parameters.cbData;
	LPBYTE params = pCertContext->pCertInfo->SubjectPublicKeyInfo.Algorithm.Parameters.pbData;

	std::vector<std::uint8_t> data(static_cast<std::size_t>(paramsLen));
	for (DWORD i = 0; i < paramsLen; ++i)
		data[i] = static_cast<std::uint8_t>(params[i]);

	return data;
}