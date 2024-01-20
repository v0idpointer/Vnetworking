#include <iostream>
#include <string>

#include <Vnetworking/Uri.h>
#include <Vnetworking/Dns/DNS.h>
#include <Vnetworking/Http/HttpRequest.h>
#include <Vnetworking/Http/HttpResponse.h>
#include <Vnetworking/Http/HttpException.h>
#include <Vnetworking/Sockets/Socket.h>
#include <Vnetworking/Sockets/IpSocketAddress.h>
#include <Vnetworking/Sockets/SocketException.h>

#pragma comment (lib, "Vnetcore.lib")
#pragma comment (lib, "Vnethttp.lib")

using namespace Vnetworking;
using namespace Vnetworking::Dns;
using namespace Vnetworking::Http;
using namespace Vnetworking::Sockets;

int main(int argc, char* argv[]) {

	std::string str;
	std::cin >> str;

	try {

		const Uri uri = { str };

		const DnsLookupResult dns = DNS::Resolve(uri.GetHost().value());
		const IpSocketAddress addr = { dns.GetAddresses()[0], 80 };

		Socket s = { AddressFamily::IPV4, SocketType::STREAM, ProtocolType::TCP };
		s.Connect(addr);

		HttpRequest req;
		req.SetMethod(HttpMethod::GET);
		req.SetRequestUri(uri);
		req.GetHeaders().AddHeader("Accept", "*/*");
		req.GetHeaders().AddHeader("Host", uri.GetHost().value());
		req.GetHeaders().AddHeader("User-Agent", "Vnetworking/1.0");

		auto data = HttpRequest::Serialize(req);
		s.Send(data, static_cast<int>(data.size()));
		s.Shutdown(ShutdownSocket::SEND);

		data.clear();
		data.resize(10000);
		s.Receive(data, static_cast<int>(data.size()));
		s.Shutdown(ShutdownSocket::RECEIVE);

		HttpResponse res = HttpResponse::Parse(data, HttpVersion::HTTP_1_1);
		std::cout << static_cast<int>(res.GetStatusCode()) << " " << ToString(res.GetStatusCode()) << std::endl;
		std::cout << "Headers:\n";
		for (const auto& [name, val] : res.GetHeaders())
			std::cout << "\t" << name << ": " << val << "\n";
		for (const char ch : res.GetPayload())
			std::cout << ch;
		std::cout << std::endl;

	}
	catch (const std::exception& ex) {
		std::cout << "*** " << ex.what() << std::endl;
	}

	return 0;
}