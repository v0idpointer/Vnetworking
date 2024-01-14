/*
	Vnetworking HTTP Library
	Copyright (C) V0idPointer
*/

#ifndef _NE_HTTP_URI_H_
#define _NE_HTTP_URI_H_

#include <Vnetworking/Exports.h>

#include <string>
#include <cstdint>
#include <optional>

namespace Vnetworking {

	class VNETHTTPAPI Uri { 
	
	private:
		std::optional<std::string> m_scheme;
		std::optional<std::string> m_userInfo;
		std::optional<std::string> m_host;
		std::optional<std::uint16_t> m_port;
		std::optional<std::string> m_path;
		std::optional<std::string> m_query;
		std::optional<std::string> m_fragment;

	public:
		Uri(const std::string& uriString);
		Uri(const Uri& uri);
		Uri(Uri&& uri) noexcept;
		virtual ~Uri(void);

		Uri& operator= (const Uri& uri);
		Uri& operator= (Uri&& uri) noexcept;
		bool operator== (const Uri& uri) const;

		std::optional<std::string> GetScheme(void) const;
		std::optional<std::string> GetUserInfo(void) const;
		std::optional<std::string> GetHost(void) const;
		std::optional<std::uint16_t> GetPort(void) const;
		std::optional<std::string> GetPath(void) const;
		std::optional<std::string> GetQuery(void) const;
		std::optional<std::string> GetFragment(void) const;
	
	};

}

#endif // _NE_HTTP_URI_H_