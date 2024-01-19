/*
	Vnetworking HTTP Library
	Copyright (C) V0idPointer
*/

#ifndef _NE_HTTP_HTTPRESPONSE_H_
#define _NE_HTTP_HTTPRESPONSE_H_

#include <Vnetworking/Exports.h>
#include <Vnetworking/Http/HttpVersion.h>
#include <Vnetworking/Http/HttpStatusCode.h>
#include <Vnetworking/Http/HttpHeaders.h>

#include <cstdint>
#include <vector>

namespace Vnetworking::Http {

	class VNETHTTPAPI HttpResponse { 
	
	private:
		HttpVersion m_version;
		HttpStatusCode m_statusCode;
		HttpHeaders m_headers;
		std::vector<std::uint8_t> m_payload;

	public:
		HttpResponse(void);
		HttpResponse(const HttpStatusCode statusCode);
		HttpResponse(const HttpVersion version, const HttpStatusCode statusCode);
		HttpResponse(const HttpResponse& httpResponse);
		HttpResponse(HttpResponse&& httpResponse) noexcept;
		virtual ~HttpResponse(void);
	
		HttpResponse& operator= (const HttpResponse& httpResponse);
		HttpResponse& operator= (HttpResponse&& httpResponse) noexcept;
		bool operator== (const HttpResponse& httpResponse) const;

		HttpVersion GetVersion(void) const;
		HttpStatusCode GetStatusCode(void) const;
		const HttpHeaders& GetHeaders(void) const;
		HttpHeaders& GetHeaders(void);
		const std::vector<std::uint8_t>& GetPayload(void) const;
		std::vector<std::uint8_t>& GetPayload(void);

		void SetVersion(const HttpVersion version);
		void SetStatusCode(const HttpStatusCode statusCode);
		void SetHeaders(const HttpHeaders& httpHeaders);
		void SetPayload(const std::vector<std::uint8_t>& payload);
		void SetPayload(std::vector<std::uint8_t>&& payload) noexcept;
		void DeletePayload(void);

		static HttpResponse Parse(const std::vector<std::uint8_t>& data, const HttpVersion version);
		static std::vector<std::uint8_t> Serialize(const HttpResponse& httpResponse);

	};

}

#endif // _NE_HTTP_HTTPRESPONSE_H_