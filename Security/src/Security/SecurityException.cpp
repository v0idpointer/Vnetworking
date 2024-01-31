#include <Vnetworking/Security/SecurityException.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

using namespace Vnetworking::Security;

static std::string GetErrorMessage(const DWORD) noexcept;

SecurityException::SecurityException(const std::int32_t errorCode)
	: std::runtime_error(GetErrorMessage(static_cast<DWORD>(errorCode))), m_errorCode(errorCode) { }

SecurityException::SecurityException(const std::int32_t errorCode, const std::string& message)
	: std::runtime_error(message), m_errorCode(errorCode) { }

SecurityException::SecurityException(const SecurityException& other) noexcept 
	: std::runtime_error(other.what()), m_errorCode(other.m_errorCode) { }

SecurityException::~SecurityException() { }

std::string SecurityException::What() const {
	return this->what();
}

std::int32_t SecurityException::GetErrorCode() const {
	return this->m_errorCode;
}

static std::string GetErrorMessage(const DWORD dwErrorCode) noexcept {

	LPSTR pszMessage = NULL;

	FormatMessageA(
		(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS),
		NULL,
		dwErrorCode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPSTR>(&pszMessage),
		NULL,
		NULL
	);

	std::string str{ pszMessage };
	LocalFree(pszMessage);
	pszMessage = NULL;

	return str;
}