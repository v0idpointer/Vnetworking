/*
	Vnetworking HTTP Library
	Copyright (C) V0idPointer
*/

#ifndef _NE_HTTP_HTTPHEADERS_H_
#define _NE_HTTP_HTTPHEADERS_H_

#include <Vnetworking/Exports.h>

#include <string>
#include <cstdint>
#include <utility>
#include <list>
#include <optional>
#include <vector>
#include <unordered_set>

namespace Vnetworking::Http {

	class VNETHTTPAPI HttpHeaders {

	private:
		std::list<std::pair<std::string, std::string>> m_headers;

	public:
		HttpHeaders(void);
		HttpHeaders(const HttpHeaders& httpHeaders);
		HttpHeaders(HttpHeaders&& httpHeaders) noexcept;
		virtual ~HttpHeaders(void);

		HttpHeaders& operator= (const HttpHeaders& httpHeaders);
		HttpHeaders& operator= (HttpHeaders&& httpHeaders) noexcept;
		bool operator== (const HttpHeaders& httpHeaders) const;

		std::list<std::pair<std::string, std::string>>::const_iterator Begin(void) const;
		std::list<std::pair<std::string, std::string>>::const_iterator begin(void) const;
		std::list<std::pair<std::string, std::string>>::const_iterator End(void) const;
		std::list<std::pair<std::string, std::string>>::const_iterator end(void) const;

		std::optional<std::string> GetHeader(std::string headerName) const;
		std::vector<std::string> GetAllHeaders(std::string headerName) const;
		bool ContainsHeader(std::string headerName) const;
		std::int32_t GetHeaderCount(std::string headerName) const;
		std::int32_t GetHeaderCount(void) const;
		std::unordered_set<std::string> GetHeaderNames(void) const;

		void AddHeader(std::string headerName, const std::string& headerValue);
		void SetHeader(std::string headerName, const std::string& headerValue);
		bool DeleteHeader(std::string headerName);
		std::int32_t DeleteAllHeaders(std::string headerName);
		void Clear(void);

		static bool IsValidHeaderName(std::string headerName);
		static bool IsValidHeaderValue(const std::string& headerValue);

	};

}

#endif // _NE_HTTP_HTTPHEADERS_H_