#include <Vnetworking/Dns/DnsLookupResult.h>

using namespace Vnetworking;
using namespace Vnetworking::Dns;

DnsLookupResult::DnsLookupResult() : m_canonicalName(), m_aliases({ }), m_addresses({ }) { }

DnsLookupResult::DnsLookupResult(
	const std::string& canonicalName, 
	const std::vector<std::string>& aliases, 
	const std::vector<IpAddress>& addresses) {

	this->m_canonicalName = canonicalName;
	this->m_aliases = std::vector<std::string>(aliases.begin(), aliases.end());
	this->m_addresses = std::vector<IpAddress>(addresses.begin(), addresses.end());

}

DnsLookupResult::DnsLookupResult(const DnsLookupResult& other) {
	this->operator= (other);
}

DnsLookupResult::DnsLookupResult(DnsLookupResult&& other) noexcept {
	this->operator= (std::move(other));
}

DnsLookupResult::~DnsLookupResult() { }

DnsLookupResult& DnsLookupResult::operator= (const DnsLookupResult& other) {
	this->m_canonicalName = other.m_canonicalName;
	this->m_aliases = std::vector<std::string>(other.m_aliases.begin(), other.m_aliases.end());
	this->m_addresses = std::vector<IpAddress>(other.m_addresses.begin(), other.m_addresses.end());
	return static_cast<DnsLookupResult&>(*this);
}

DnsLookupResult& DnsLookupResult::operator= (DnsLookupResult&& other) noexcept {
	this->m_canonicalName = std::move(other.m_canonicalName);
	this->m_aliases = std::move(other.m_aliases);
	this->m_addresses = std::move(other.m_addresses);
	return static_cast<DnsLookupResult&>(*this);
}

bool DnsLookupResult::operator== (const DnsLookupResult& other) const {
	if (this->m_canonicalName != other.m_canonicalName) return false;
	if (this->m_aliases != other.m_aliases) return false;
	if (this->m_addresses != other.m_addresses) return false;
	return true;
}

std::string DnsLookupResult::GetCanonicalName() const {
	return this->m_canonicalName;
}

const std::vector<std::string>& DnsLookupResult::GetAliases() const {
	return this->m_aliases;
}

const std::vector<IpAddress>& DnsLookupResult::GetAddresses() const {
	return this->m_addresses;
}

std::vector<std::string>& DnsLookupResult::GetAliases() {
	return this->m_aliases;
}

std::vector<IpAddress>& DnsLookupResult::GetAddresses() {
	return this->m_addresses;
}

void DnsLookupResult::SetCanonicalName(const std::string& canonicalName) {
	this->m_canonicalName = canonicalName;
}

void DnsLookupResult::SetAliases(const std::vector<std::string>& aliases) {
	this->m_aliases = aliases;
}

void DnsLookupResult::SetAddresses(const std::vector<IpAddress>& addresses) {
	this->m_addresses = addresses;
}