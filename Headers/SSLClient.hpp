/// SSLClent.hpp - Header file for the "SSL Client" stuff.
/// Dec 12, 2021
/// Chris M.
/// https://github.com/RealTimeChris

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"

#ifdef _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <io.h>
#pragma comment(lib,"ws2_32")
#endif

#pragma comment(lib, "openssl")
#pragma comment(lib, "libssl")

#include <openssl/ssl.h>
#include <openssl/err.h>

namespace DiscordCoreInternal {

	class MsgWebSocketSSLContext {
	public:

		SSL_CTX* context{ nullptr };

		SSL* ssl{ nullptr };

	};

	class DiscordCoreAPI_Dll MsgWebSocketSSLClient {
	public:
		MsgWebSocketSSLClient(string, string);

		~MsgWebSocketSSLClient();

	protected:
		unique_ptr<MsgWebSocketSSLContext> ssl{ make_unique<MsgWebSocketSSLContext>() };
		uint32_t sfd{ static_cast<uint32_t>(~0) };
		vector<uint8_t> outputBuffer{};
		vector<uint8_t> inputBuffer{};
		bool nonblocking{ false };
		uint64_t bytesOut{ 0 };
		uint64_t lastTime{ 0 };
		uint64_t bytesIn{ 0 };
		string hostname{ "" };
		string cipher{ "" };
		string port{ "" };
	};

}