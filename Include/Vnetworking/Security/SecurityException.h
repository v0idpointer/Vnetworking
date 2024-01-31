/*
	Vnetworking SSL/TLS Library
	Copyright (C) V0idPointer
*/

#ifndef _NE_SECURITY_SECURITYEXCEPTION_H_
#define _NE_SECURITY_SECURITYEXCEPTION_H_

#include <Vnetworking/Exports.h>

#include <string>
#include <cstdint>
#include <exception>
#include <stdexcept>

namespace Vnetworking::Security {

	class VNETSECURITYAPI SecurityException : public std::runtime_error {

	private:
		std::int32_t m_errorCode;

	public:
		SecurityException(const std::int32_t errorCode);
		SecurityException(const std::int32_t errorCode, const std::string& message);
		SecurityException(const SecurityException& other) noexcept;
		virtual ~SecurityException(void);

		std::string What(void) const;
		std::int32_t GetErrorCode(void) const;

	};

}

#endif // _NE_SECURITY_SECURITYEXCEPTION_H_