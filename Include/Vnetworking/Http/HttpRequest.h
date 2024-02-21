/*
	Vnetworking HTTP Library
	Copyright (C) V0idPointer
*/

#ifndef _NE_HTTP_HTTPREQUEST_H_
#define _NE_HTTP_HTTPREQUEST_H_

#include <Vnetworking/Exports.h>
#include <Vnetworking/Uri.h>
#include <Vnetworking/Http/HttpMethod.h>
#include <Vnetworking/Http/HttpHeaders.h>

#include <string>
#include <cstdint>
#include <vector>
#include <span>

namespace Vnetworking::Http {

	class VNETHTTPAPI HttpRequest {

	private:
		Uri m_requestUri;
		HttpMethod m_method;
		HttpHeaders m_headers;
		std::vector<std::uint8_t> m_payload;

	public:
		HttpRequest(void);
		HttpRequest(const HttpMethod method, const Uri& uri);
		HttpRequest(const HttpMethod method, const std::string& uri);
		HttpRequest(const HttpRequest& httpRequest);
		HttpRequest(HttpRequest&& httpRequest) noexcept;
		virtual ~HttpRequest(void);

		HttpRequest& operator= (const HttpRequest& httpRequest);
		HttpRequest& operator= (HttpRequest&& httpRequest) noexcept;
		bool operator== (const HttpRequest& httpRequest) const;

		const Uri& GetRequestUri(void) const;
		HttpMethod GetMethod(void) const;
		const HttpHeaders& GetHeaders(void) const;
		HttpHeaders& GetHeaders(void);
		const std::vector<std::uint8_t>& GetPayload(void) const;
		std::vector<std::uint8_t>& GetPayload(void);

		void SetRequestUri(const Uri& uri);
		void SetRequestUri(Uri&& uri) noexcept;
		void SetRequestUri(const std::string& uri);
		void SetMethod(const HttpMethod method);
		void SetHeaders(const HttpHeaders& httpHeaders);
		void SetHeaders(HttpHeaders&& httpHeaders) noexcept;
		void SetPayload(const std::span<const std::uint8_t>& payload);
		void SetPayload(std::vector<std::uint8_t>&& payload) noexcept;
		void DeletePayload(void);
		void Write(const std::string& text);

		static HttpRequest Parse(const std::span<const std::uint8_t>& data);
		static std::vector<std::uint8_t> Serialize(const HttpRequest& httpRequest);

	};

}

#endif // _NE_HTTP_HTTPREQUEST_H_