#include <Vnetworking/Date.h>

#include <sstream>
#include <format>

using namespace Vnetworking;

constexpr std::string_view DAY_NAMES[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", };
constexpr std::string_view MONTH_NAMES[] = { 
	"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec",
};

Date::Date() : Date(std::chrono::system_clock::now()) { }

Date::Date(const std::time_t time) : m_time(time) { }

Date::Date(const std::chrono::time_point<std::chrono::system_clock> timePoint)
	: Date(std::chrono::system_clock::to_time_t(timePoint)) { }

Date::Date(const Date& date) {
	this->operator= (date);
}

Date::Date(Date&& date) noexcept {
	this->operator= (std::move(date));
}

Date::~Date() { }

Date& Date::operator= (const Date& date) {
	this->m_time = date.m_time;
	return static_cast<Date&>(*this);
}

Date& Date::operator= (Date&& date) noexcept {
	this->m_time = date.m_time;
	return static_cast<Date&>(*this);
}

Date Date::operator+ (const Date& date) const {
	return (this->m_time + date.m_time);
}

Date Date::operator- (const Date& date) const {
	return (this->m_time - date.m_time);
}

Date& Date::operator+= (const Date& date) {
	this->m_time += date.m_time;
	return static_cast<Date&>(*this);
}

Date& Date::operator-= (const Date& date) {
	this->m_time -= date.m_time;
	return static_cast<Date&>(*this);
}

bool Date::operator== (const Date& date) const {
	return (this->m_time == date.m_time);
}

bool Date::operator> (const Date& date) const {
	return (this->m_time > date.m_time);
}

bool Date::operator>= (const Date& date) const {
	return (this->m_time >= date.m_time);
}

bool Date::operator< (const Date& date) const {
	return (this->m_time < date.m_time);
}

bool Date::operator<= (const Date& date) const {
	return (this->m_time <= date.m_time);
}

static inline std::tm GetTm(const std::time_t* const pTime) noexcept {
	std::tm tm;
	localtime_s(&tm, pTime);
	return tm;
}

std::int32_t Date::GetDate() const {
	return GetTm(&this->m_time).tm_mday;
}

std::int32_t Date::GetDay(void) const {
	return GetTm(&this->m_time).tm_wday;
}

std::int32_t Date::GetFullYear() const {
	return (GetTm(&this->m_time).tm_year + 1900);
}

std::int32_t Date::GetHours() const {
	return GetTm(&this->m_time).tm_hour;
}

std::int32_t Date::GetMinutes() const {
	return GetTm(&this->m_time).tm_min;
}

std::int32_t Date::GetMonth() const {
	return GetTm(&this->m_time).tm_mon;
}

std::int32_t Date::GetSeconds() const {
	return GetTm(&this->m_time).tm_sec;
}

std::time_t Date::GetTime() const {
	return this->m_time;
}

std::int32_t Date::GetTimezoneOffset() const {

	const std::tm tm = GetTm(&this->m_time);
	const std::chrono::zoned_time zt = { std::chrono::current_zone(), std::chrono::system_clock::from_time_t(this->m_time) };
	const std::chrono::sys_info info = zt.get_info();
	const auto offset = std::chrono::duration_cast<std::chrono::minutes>(info.offset).count();

	return static_cast<std::int32_t>(offset);
}

void Date::SetTime(const std::time_t time) {
	this->m_time = time;
}

void Date::SetTime(const std::chrono::time_point<std::chrono::system_clock> timePoint) {
	this->m_time = std::chrono::system_clock::to_time_t(timePoint);
}

std::string Date::ToString() const {

	std::ostringstream stream;

	const std::tm tm = GetTm(&this->m_time);
	const std::chrono::zoned_time zt = { std::chrono::current_zone(), std::chrono::system_clock::from_time_t(this->m_time) };
	const std::chrono::sys_info info = zt.get_info();
	const auto hourOffset = std::chrono::duration_cast<std::chrono::hours>(info.offset).count();
	const auto minuteOffset = std::chrono::duration_cast<std::chrono::hours>(info.offset % std::chrono::hours(1)).count();

	stream << DAY_NAMES[tm.tm_wday] << " ";
	stream << MONTH_NAMES[tm.tm_mon] << " ";
	stream << std::format("{:02}", tm.tm_mday) << " ";
	stream << (tm.tm_year + 1900) << " ";
	stream << std::format("{:02}", tm.tm_hour) << ":";
	stream << std::format("{:02}", tm.tm_min) << ":";
	stream << std::format("{:02}", tm.tm_sec) << " ";
	stream << std::format("GMT+{0:02}{1:02}", static_cast<int>(hourOffset), static_cast<int>(minuteOffset));

	return stream.str();
}

std::string Date::ToUTCString() const {

	std::ostringstream stream;

	std::tm tm;
	gmtime_s(&tm, &this->m_time);

	stream << DAY_NAMES[tm.tm_wday] << ", ";
	stream << std::format("{:02}", tm.tm_mday) << " ";
	stream << MONTH_NAMES[tm.tm_mon] << " ";
	stream << (tm.tm_year + 1900) << " ";
	stream << std::format("{:02}", tm.tm_hour) << ":";
	stream << std::format("{:02}", tm.tm_min) << ":";
	stream << std::format("{:02}", tm.tm_sec) << " GMT";

	return stream.str();
}

std::string Date::ToISO8601String() const {

	std::ostringstream stream;

	std::tm tm;
	gmtime_s(&tm, &this->m_time);

	stream << (tm.tm_year + 1900) << "-";
	stream << std::format("{:02}", (tm.tm_mon + 1)) << "-";
	stream << std::format("{:02}", tm.tm_mday) << "T";
	stream << std::format("{:02}", tm.tm_hour) << ":";
	stream << std::format("{:02}", tm.tm_min) << ":";
	stream << std::format("{:02}", tm.tm_sec) << ".000Z";

	return stream.str();
}

Date Date::Now() {
	return Date();
}

Date Date::MaxDate() {
	return Date(2147483647);
}

Date Date::MinDate() {
	return Date(0);
}