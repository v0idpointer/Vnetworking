/*
	Vnetworking HTTP Library
	Copyright (C) V0idPointer
*/

#ifndef _NE_HTTP_BADURIEXCEPTION_H_
#define _NE_HTTP_BADURIEXCEPTION_H_

#include <Vnetworking/Exports.h>

#include <string>
#include <exception>
#include <stdexcept>

namespace Vnetworking {

	class VNETHTTPAPI BadUriException : public std::runtime_error {

	public:
		BadUriException(const std::string& message);
		BadUriException(const BadUriException& other) noexcept;
		virtual ~BadUriException(void);

		std::string What(void) const;

	};

}

#endif // _NE_HTTP_BADURIEXCEPTION_H_