/*
	Vnetworking HTTP Library
	Copyright (C) V0idPointer
*/

#ifndef _NE_HTTP_DATE_H_
#define _NE_HTTP_DATE_H_

#include <Vnetworking/Exports.h>

#include <string>
#include <cstdint>
#include <chrono>

namespace Vnetworking {

	class VNETHTTPAPI Date {

	private:
		std::time_t m_time;

	public:
		Date(void);
		Date(const std::time_t time);
		Date(const std::chrono::time_point<std::chrono::system_clock> timePoint);
		Date(const Date& date);
		Date(Date&& date) noexcept;
		virtual ~Date(void);

		Date& operator= (const Date& date);
		Date& operator= (Date&& date) noexcept;
		Date operator+ (const Date& date) const;
		Date operator- (const Date& date) const;
		Date& operator+= (const Date& date);
		Date& operator-= (const Date& date);
		bool operator== (const Date& date) const;
		bool operator> (const Date& date) const;
		bool operator>= (const Date& date) const;
		bool operator< (const Date& date) const;
		bool operator<= (const Date& date) const;

		template<typename Rep, typename Period>
		Date operator+ (const std::chrono::duration<Rep, Period> duration) const {
			return (this->m_time + static_cast<std::time_t>(duration.count() * Period::num / Period::den));
		}

		template<typename Rep, typename Period>
		Date operator- (const std::chrono::duration<Rep, Period> duration) const {
			return (this->m_time - static_cast<std::time_t>(duration.count() * Period::num / Period::den));
		}

		template<typename Rep, typename Period>
		Date& operator+= (const std::chrono::duration<Rep, Period> duration) {
			this->m_time += static_cast<std::time_t>(duration.count() * Period::num / Period::den);
			return static_cast<Date&>(*this);
		}

		template<typename Rep, typename Period>
		Date& operator-= (const std::chrono::duration<Rep, Period> duration) {
			this->m_time -= static_cast<std::time_t>(duration.count() * Period::num / Period::den);
			return static_cast<Date&>(*this);
		}

		std::int32_t GetDate(void) const;
		std::int32_t GetDay(void) const;
		std::int32_t GetFullYear(void) const;
		std::int32_t GetHours(void) const;
		std::int32_t GetMinutes(void) const;
		std::int32_t GetMonth(void) const;
		std::int32_t GetSeconds(void) const;
		std::time_t GetTime(void) const;
		std::int32_t GetTimezoneOffset(void) const;

		void SetTime(const std::time_t time);
		void SetTime(const std::chrono::time_point<std::chrono::system_clock> timePoint);

		std::string ToString(void) const;
		std::string ToUTCString(void) const;
		std::string ToISO8601String(void) const;

		static Date Now(void);
		static Date MaxDate(void);
		static Date MinDate(void);

	};

}

#endif // _NE_HTTP_DATE_H_