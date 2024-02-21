/*
	Vnetworking HTTP Library
	Copyright (C) V0idPointer
*/

#ifndef _NE_HTTP_HTTPEXCEPTION_H_
#define _NE_HTTP_HTTPEXCEPTION_H_

#include <Vnetworking/Exports.h>

#include <string>
#include <cstdint>
#include <exception>
#include <stdexcept>

namespace Vnetworking::Http {

	enum class VNETHTTPAPI HttpErrorType : std::uint16_t { 
	
		REQUEST_PARSING_ERROR = 1,
		REQUEST_SERIALIZATION_ERROR = 2,
		RESPONSE_PARSING_ERROR = 3,
		RESPONSE_SERIALIZATION_ERROR = 4,

	};

	enum class VNETHTTPAPI HttpErrorSubtype : std::uint16_t { 
	
		GENERIC_ERROR = 0,
		INVALID_STATUS_CODE = 1,
		INVALID_HEADER_NAME = 2,
		INVALID_HEADER_VALUE = 3,
		INVALID_HTTP_VERSION = 4,
		INVALID_METHOD = 5,
		INVALID_REQUEST_URI = 6,
	
	};

	VNETHTTPAPI std::string ToString(const HttpErrorType type);
	VNETHTTPAPI std::string ToString(const HttpErrorSubtype subtype);

	class VNETHTTPAPI HttpException : public std::runtime_error {
		
	private:
		HttpErrorType m_errType;
		HttpErrorSubtype m_errSubtype;

	public:
		HttpException(const HttpErrorType type, const HttpErrorSubtype subtype);
		HttpException(const HttpErrorType type, const HttpErrorSubtype subtype, const std::string& message);
		HttpException(const HttpException& other) noexcept;
		virtual ~HttpException(void);

		std::string What(void) const;
		HttpErrorType GetErrorType(void) const;
		HttpErrorSubtype GetErrorSubtype(void) const;

	};

}

#endif // _NE_HTTP_HTTPEXCEPTION_H_