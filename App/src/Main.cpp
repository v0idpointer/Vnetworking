#include <iostream>
#include <string>

#include <Vnetworking/ThreadPool.h>
#include <Vnetworking/Uri.h>
#include <Vnetworking/Dns/DNS.h>
#include <Vnetworking/Http/HttpMethod.h>
#include <Vnetworking/Http/HttpStatusCode.h>
#include <Vnetworking/Http/HttpVersion.h>
#include <Vnetworking/Sockets/Socket.h>
#include <Vnetworking/Sockets/IpSocketAddress.h>
#include <Vnetworking/Sockets/SocketException.h>
#include <Vnetworking/Security/SecurityException.h>
#include <Vnetworking/Security/Certificates/Certificate.h>
#include <Vnetworking/Security/SecureConnection.h>
#include <Vnetworking/Security/SecurityContext.h>

#pragma comment (lib, "Vnetcore.lib")
#pragma comment (lib, "Vnethttp.lib")
#pragma comment (lib, "Vnetsec.lib")

using namespace Vnetworking;
using namespace Vnetworking::Dns;
using namespace Vnetworking::Http;
using namespace Vnetworking::Sockets;
using namespace Vnetworking::Security;
using namespace Vnetworking::Security::Certificates;

static inline std::vector<std::uint8_t> ToByteBuffer(const std::string& str) noexcept {
	std::vector<std::uint8_t> data(str.length());
	memcpy_s(data.data(), data.size(), str.c_str(), str.length());
	return data;
}

int main(int argc, char* argv[]) {

	

	return 0;
}