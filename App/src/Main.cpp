#include <iostream>
#include <string>

#include <Vnetworking/ThreadPool.h>
#include <Vnetworking/Uri.h>
#include <Vnetworking/BadUriException.h>
#include <Vnetworking/Dns/DNS.h>
#include <Vnetworking/Http/HttpMethod.h>
#include <Vnetworking/Http/HttpStatusCode.h>
#include <Vnetworking/Http/HttpHeaders.h>
#include <Vnetworking/Http/HttpRequest.h>
#include <Vnetworking/Http/HttpResponse.h>
#include <Vnetworking/Http/HttpException.h>
#include <Vnetworking/Sockets/Socket.h>
#include <Vnetworking/Sockets/IpSocketAddress.h>
#include <Vnetworking/Sockets/SocketException.h>
#include <Vnetworking/Security/SecurityException.h>
#include <Vnetworking/Security/Certificates/Certificate.h>
#include <Vnetworking/Security/SecureConnection.h>
#include <Vnetworking/Security/SecurityContext.h>

#include <Vnetworking/Http/HttpCookie.h>

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

	HttpCookie cookie = { "SessionID", "7355608" };
	cookie.SetExpirationDate(Date::Now() + std::chrono::days(7));
	cookie.SetDomain("example.net");
	cookie.SetPath("/api");
	cookie.SetHttpOnly(true);

	std::cout << cookie.ToString() << "\n";

	return 0;
}