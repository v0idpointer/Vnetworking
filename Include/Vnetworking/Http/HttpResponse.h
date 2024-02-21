/*
	Vnetworking HTTP Library
	Copyright (C) V0idPointer
*/

#ifndef _NE_HTTP_HTTPRESPONSE_H_
#define _NE_HTTP_HTTPRESPONSE_H_

#include <Vnetworking/Exports.h>
#include <Vnetworking/Http/HttpStatusCode.h>
#include <Vnetworking/Http/HttpHeaders.h>

#include <string>
#include <cstdint>
#include <vector>
#include <span>

namespace Vnetworking::Http {

	class VNETHTTPAPI HttpResponse {

	private:
		HttpStatusCode m_statusCode;
		HttpHeaders m_headers;
		std::vector<std::uint8_t> m_payload;

	public:
		HttpResponse(void);
		HttpResponse(const std::uint32_t statusCode);
		HttpResponse(const HttpStatusCode statusCode);
		HttpResponse(const HttpResponse& httpResponse);
		HttpResponse(HttpResponse&& httpResponse) noexcept;
		virtual ~HttpResponse(void);

		HttpResponse& operator= (const HttpResponse& httpResponse);
		HttpResponse& operator= (HttpResponse&& httpResponse) noexcept;
		bool operator== (const HttpResponse& httpResponse) const;

		HttpStatusCode GetStatusCode(void) const;
		const HttpHeaders& GetHeaders(void) const;
		HttpHeaders& GetHeaders(void);
		const std::vector<std::uint8_t>& GetPayload(void) const;
		std::vector<std::uint8_t>& GetPayload(void);

		void SetStatusCode(const std::uint32_t statusCode);
		void SetStatusCode(const HttpStatusCode statusCode);
		void SetHeaders(const HttpHeaders& httpHeaders);
		void SetHeaders(HttpHeaders&& httpHeaders) noexcept;
		void SetPayload(const std::span<const std::uint8_t>& payload);
		void SetPayload(std::vector<std::uint8_t>&& payload) noexcept;
		void DeletePayload(void);
		void Write(const std::string& text);

		static HttpResponse Parse(const std::span<const std::uint8_t>& data);
		static std::vector<std::uint8_t> Serialize(const HttpResponse& httpResponse);

	};

}

#endif // _NE_HTTP_HTTPRESPONSE_H_