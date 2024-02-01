#include <Vnetworking/Security/Certificate.h>
#include <Vnetworking/Security/SecurityException.h>

#include <fstream>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <wincrypt.h>

#pragma comment (lib, "crypt32.lib")

using namespace Vnetworking::Security;

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

Certificate::Certificate(Certificate&& cert) noexcept {
	this->operator= (std::move(cert));
}

Certificate::~Certificate() {

	if (this->m_certificateContext) {
		PCCERT_CONTEXT pCertContext = reinterpret_cast<PCCERT_CONTEXT>(this->m_certificateContext);
		CertFreeCertificateContext(pCertContext);
	}

	this->m_certificateContext = nullptr;

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

std::int32_t Certificate::GetVersion() const {
	PCCERT_CONTEXT pCertContext = reinterpret_cast<PCCERT_CONTEXT>(this->m_certificateContext);
	return (pCertContext->pCertInfo->dwVersion + 1);
}

std::string Certificate::GetSubject() const {
	return "/";
}

std::string Certificate::GetIssuer() const {
	return "/";
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