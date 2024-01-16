/*
	Vnetworking HTTP Library
	Copyright (C) V0idPointer
*/

#ifndef _NE_HTTP_HTTPREQUEST_H_
#define _NE_HTTP_HTTPREQUEST_H_

#include <Vnetworking/Exports.h>
#include <Vnetworking/Uri.h>
#include <Vnetworking/Http/HttpVersion.h>
#include <Vnetworking/Http/HttpMethod.h>
#include <Vnetworking/Http/HttpHeaders.h>

#include <cstdint>
#include <vector>

namespace Vnetworking::Http {

	class VNETHTTPAPI HttpRequest { 
	
	private:
		HttpVersion m_version;
		HttpMethod m_method;
		Uri m_requestUri;
		HttpHeaders m_headers;
		std::vector<std::uint8_t> m_payload;

	public:
		HttpRequest(void);
		HttpRequest(const HttpMethod method, const Uri& requestUri);
		HttpRequest(const HttpVersion version, const HttpMethod method, const Uri& requestUri);
		HttpRequest(const HttpRequest& httpRequest);
		HttpRequest(HttpRequest&& httpRequest) noexcept;
		virtual ~HttpRequest(void);

		HttpRequest& operator= (const HttpRequest& httpRequest);
		HttpRequest& operator= (HttpRequest&& httpRequest) noexcept;
		bool operator== (const HttpRequest& httpRequest) const;
	
		HttpVersion GetVersion(void) const;
		HttpMethod GetMethod(void) const;
		const Uri& GetRequestUri(void) const;
		const HttpHeaders& GetHeaders(void) const;
		HttpHeaders& GetHeaders(void);
		const std::vector<std::uint8_t>& GetPayload(void) const;
		std::vector<std::uint8_t>& GetPayload(void);

		void SetVersion(const HttpVersion version);
		void SetMethod(const HttpMethod method);
		void SetRequestUri(const Uri& requestUri);
		void SetHeaders(const HttpHeaders& httpHeaders);
		void SetPayload(const std::vector<std::uint8_t>& payload);
		void SetPayload(std::vector<std::uint8_t>&& payload) noexcept;
		void DeletePayload(void);

		// static HttpRequest Parse(const std::vector<std::uint8_t>& data);
		static std::vector<std::uint8_t> Serialize(const HttpRequest& httpRequest);

	};

}

#endif // _NE_HTTP_HTTPREQUEST_H_