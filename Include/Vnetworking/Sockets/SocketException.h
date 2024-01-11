/*
	Vnetworking Library
	Copyright (C) V0idPointer
*/

#ifndef _NE_SOCKETEXCEPTION_H_
#define _NE_SOCKETEXCEPTION_H_

#include <Vnetworking/Exports.h>

#include <string>
#include <cstdint>
#include <exception>
#include <stdexcept>

namespace Vnetworking::Sockets {

	class VNETCOREAPI SocketException : public std::runtime_error {

	private:
		std::int32_t m_errorCode;

	public:
		SocketException(const std::int32_t errorCode);
		SocketException(const std::int32_t errorCode, const std::string& message);
		SocketException(const SocketException& other) noexcept;
		virtual ~SocketException(void);

		std::string What(void) const;
		std::int32_t GetErrorCode(void) const;

	};

}

#endif // _NE_SOCKETEXCEPTION_H_