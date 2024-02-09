#include <iostream>
#include <string>

#include <Vnetworking/ThreadPool.h>
#include <Vnetworking/Uri.h>
#include <Vnetworking/Dns/DNS.h>
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

#pragma comment (lib, "Vnetcore.lib")
#pragma comment (lib, "Vnethttp.lib")
#pragma comment (lib, "Vnetsec.lib")

using namespace Vnetworking;
using namespace Vnetworking::Dns;
using namespace Vnetworking::Http;
using namespace Vnetworking::Sockets;
using namespace Vnetworking::Security;
using namespace Vnetworking::Security::Certificates;

static std::string HTML_TEXT = {
	"<style>\n"
	"body {\n"
	"color: white;\n"
	"background-color: #232323;\n"
	"}\n"
	"</style>\n"
	"<h3>Hello World!</h3>"
};

static inline std::vector<std::uint8_t> ToByteBuffer(const std::string& str) noexcept {
	std::vector<std::uint8_t> data(str.length());
	memcpy_s(data.data(), data.size(), str.c_str(), str.length());
	return data;
}

int main(int argc, char* argv[]) {

	const std::filesystem::path certPath = R"(C:\Users\Mastermind\Desktop\test.pfx)";
	const std::string certPassword = "aaa";
	const Port port = 443;

	std::optional<Socket> socket;
	std::optional<Certificate> cert;
	std::optional<SecurityContext> ctx;

	try { socket = { AddressFamily::IPV4, SocketType::STREAM, ProtocolType::TCP }; }
	catch (const SocketException& ex) {
		std::cout << ex.What() << "\n";
		return 1;
	}

	try { cert = { certPath, certPassword }; }
	catch (const SecurityException& ex) {
		std::cout << ex.What() << "\n";
		return 2;
	}

	try { ctx = { cert, SecurityProtocol::TLS_1_2, ApplicationRole::SERVER }; }
	catch (const SecurityException& ex) {
		std::cout << ex.What() << "\n";
		return 3;
	}

	IpSocketAddress sockaddr = { IpAddress::Any(), port };
	socket->Bind(sockaddr);
	socket->Listen();

	while (true) {

		Socket c = socket->Accept();
		std::optional<SecureConnection> s = ctx->AcceptConnection(c.GetNativeSocketHandle());
		if (!s.has_value()) continue;

		HttpResponse res = { HttpVersion::HTTP_1_1, HttpStatusCode::OK };
		res.SetPayload(ToByteBuffer(HTML_TEXT));
		res.GetHeaders().SetHeader("Content-Length", std::to_string(res.GetPayload().size()));
		res.GetHeaders().SetHeader("Content-Type", "text/html; charset=UTF-8");

		std::vector<std::uint8_t> data = HttpResponse::Serialize(res);
		data = s->Encrypt(data);
		c.Send(data, static_cast<std::int32_t>(data.size()));

		std::this_thread::sleep_for(std::chrono::seconds(1));
		c.Close();

	}

	return 0;
}