/*
	Vnetworking HTTP Library
	Copyright (C) V0idPointer
*/

#ifndef _NE_HTTP_HTTPEXCEPTION_H_
#define _NE_HTTP_HTTPEXCEPTION_H_

#include <Vnetworking/Exports.h>
#include <Vnetworking/Http/HttpStatusCode.h>

#include <string>
#include <exception>
#include <stdexcept>

namespace Vnetworking::Http {

	class VNETHTTPAPI HttpException : public std::runtime_error {

	private:
		HttpStatusCode m_statusCode;

	public:
		HttpException(const std::string& message);
		HttpException(const HttpStatusCode statusCode);
		HttpException(const HttpStatusCode statusCode, const std::string& message);
		HttpException(const HttpException& other) noexcept;
		virtual ~HttpException(void);

		std::string What(void) const;
		HttpStatusCode GetStatusCode(void) const;

	};

}

#endif // _NE_HTTP_HTTPEXCEPTION_H_