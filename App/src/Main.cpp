#include <iostream>
#include <string>

#include <Vnetworking/Dns/DNS.h>
#include <Vnetworking/Dns/DnsLookupResult.h>
#include <Vnetworking/Sockets/SocketException.h>

#pragma comment (lib, "Vnetcore.lib")
#pragma comment (lib, "Vnethttp.lib")

using namespace Vnetworking;
using namespace Vnetworking::Dns;
using namespace Vnetworking::Sockets;

int main(int argc, char* argv[]) {

	while (true) {

		std::cout << "> ";
		std::string host;
		std::cin >> host;

		DnsLookupResult dns;
		try { dns = DNS::Resolve(host); }
		catch (const SocketException& ex) {
			std::cout << "*** SocketException: " << ex.what() << std::endl;
			continue;
		}

		std::cout << "Name: " << dns.GetCanonicalName() << std::endl;
		std::cout << "Aliases:";
		for (const std::string& alias : dns.GetAliases()) std::cout << "\n\t" << alias;
		std::cout << "\nAddresses:";
		for (const IpAddress& addr : dns.GetAddresses()) std::cout << "\n\t" << addr.ToString();
		std::cout << std::endl;

	}

	return 0;
}