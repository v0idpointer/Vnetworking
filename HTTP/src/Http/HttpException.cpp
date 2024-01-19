#include <Vnetworking/Http/HttpException.h>

using namespace Vnetworking::Http;

HttpException::HttpException(const std::string& message)
	: std::runtime_error(message), m_statusCode(HttpStatusCode::INTERNAL_SERVER_ERROR) { }

HttpException::HttpException(const HttpStatusCode statusCode)
	: std::runtime_error(ToString(statusCode)), m_statusCode(statusCode) { }

HttpException::HttpException(const HttpStatusCode statusCode, const std::string& message)
	: std::runtime_error(message), m_statusCode(statusCode) { }

HttpException::HttpException(const HttpException& other) noexcept
	: std::runtime_error(other.what()), m_statusCode(other.m_statusCode) { }

HttpException::~HttpException() { }

std::string HttpException::What() const {
	return this->what();
}

HttpStatusCode HttpException::GetStatusCode() const {
	return this->m_statusCode;
}