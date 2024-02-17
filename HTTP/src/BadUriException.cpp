#include <Vnetworking/BadUriException.h>

using namespace Vnetworking;

BadUriException::BadUriException(const std::string& message)
	: std::runtime_error(message) { }

BadUriException::BadUriException(const BadUriException& other) noexcept
	: BadUriException(other.what()) { }

BadUriException::~BadUriException() { }

std::string BadUriException::What() const {
	return this->what();
}