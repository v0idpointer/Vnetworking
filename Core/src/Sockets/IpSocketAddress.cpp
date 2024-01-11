#include <Vnetworking/Sockets/IpSocketAddress.h>

using namespace Vnetworking;
using namespace Vnetworking::Sockets;

IpSocketAddress::IpSocketAddress() : m_af(AddressFamily::IPV4), m_ipAddress({ 0, 0, 0, 0 }), m_port(0) { }

IpSocketAddress::IpSocketAddress(const IpAddress& ipAddress, const Port port) 
	: m_af( ipAddress.IsVersion6() ? AddressFamily::IPV6 : AddressFamily::IPV4 ), 
	m_ipAddress(ipAddress), 
	m_port(port) { }

IpSocketAddress::IpSocketAddress(const IpSocketAddress& sockaddr) {
	this->operator= (sockaddr);
}

IpSocketAddress::IpSocketAddress(IpSocketAddress&& sockaddr) noexcept {
	this->operator= (std::move(sockaddr));
}

IpSocketAddress::~IpSocketAddress() { }

IpSocketAddress& IpSocketAddress::operator= (const IpSocketAddress& sockaddr) {
	this->m_af = sockaddr.m_af;
	this->m_ipAddress = sockaddr.m_ipAddress;
	this->m_port = sockaddr.m_port;
	return static_cast<IpSocketAddress&>(*this);
}

IpSocketAddress& IpSocketAddress::operator= (IpSocketAddress&& sockaddr) noexcept {
	this->m_af = std::move(sockaddr.m_af);
	this->m_ipAddress = std::move(sockaddr.m_ipAddress);
	this->m_port = std::move(sockaddr.m_port); 
	return static_cast<IpSocketAddress&>(*this);
}

bool IpSocketAddress::operator== (const IpSocketAddress& sockaddr) const {
	return ((this->m_af == sockaddr.m_af) && (this->m_ipAddress == sockaddr.m_ipAddress) && (this->m_port == sockaddr.m_port));
}

AddressFamily IpSocketAddress::GetAddressFamily() const {
	return this->m_af;
}

IpAddress IpSocketAddress::GetIpAddress() const {
	return this->m_ipAddress;
}

Port IpSocketAddress::GetPort() const {
	return this->m_port;
}

void IpSocketAddress::SetIpAddress(const IpAddress& ipAddress) {
	this->m_ipAddress = ipAddress;
	this->m_af = (ipAddress.IsVersion6() ? AddressFamily::IPV6 : AddressFamily::IPV4);
}

void IpSocketAddress::SetPort(const Port port) {
	this->m_port = port;
}