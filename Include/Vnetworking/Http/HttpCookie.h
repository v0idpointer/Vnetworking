/*
	Vnetworking HTTP Library
	Copyright (C) V0idPointer
*/

#ifndef _NE_HTTP_HTTPCOOKIE_H_
#define _NE_HTTP_HTTPCOOKIE_H_

#include <Vnetworking/Exports.h>
#include <Vnetworking/Date.h>

#include <string>
#include <cstdint>
#include <optional>

namespace Vnetworking::Http {

	class VNETHTTPAPI HttpCookie {

	private:
		std::string m_name;
		std::string m_value;
		std::optional<Date> m_expiration;
		std::optional<std::int32_t> m_maxAge;
		std::optional<std::string> m_domain;
		std::optional<std::string> m_path;
		bool m_secure;
		bool m_httpOnly;
		std::optional<std::string> m_sameSite;

	public:
		HttpCookie(void);
		HttpCookie(const std::string& name, const std::string& value);
		HttpCookie(const HttpCookie& httpCookie);
		HttpCookie(HttpCookie&& httpCookie) noexcept;
		virtual ~HttpCookie(void);

		HttpCookie& operator= (const HttpCookie& httpCookie);
		HttpCookie& operator= (HttpCookie&& httpCookie) noexcept;
		bool operator== (const HttpCookie& httpCookie) const;

		std::string GetName(void) const;
		std::string GetValue(void) const;
		std::optional<Date> GetExpirationDate(void) const;
		std::optional<std::int32_t> GetMaxAge(void) const;
		std::optional<std::string> GetDomain(void) const;
		std::optional<std::string> GetPath(void) const;
		bool IsSecure(void) const;
		bool IsHttpOnly(void) const;
		std::optional<std::string> GetSameSite(void) const;

		void SetName(const std::string& name);
		void SetValue(const std::string& value);
		void SetExpirationDate(const std::optional<Date>& expirationDate);
		void SetMaxAge(const std::optional<std::int32_t> maxAge);
		void SetDomain(const std::optional<std::string>& domain);
		void SetPath(const std::optional<std::string>& path);
		void SetSecure(const bool secure);
		void SetHttpOnly(const bool httpOnly);
		void SetSameSite(const std::optional<std::string>& sameSite);

		std::string ToString(void) const;

	};

}

#endif // _NE_HTTP_HTTPCOOKIE_H_