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

	struct DiscordCoreAPI_Dll MsgWebSocketSSLContext {

		SSL_CTX* context{ nullptr };

		SSL* ssl{ nullptr };

	};

	class DiscordCoreAPI_Dll MsgWebSocketSSLClient {
	public:

		function<uint32_t()> writeableCallBack{};
		function<void()> writeableNotification{};
		function<uint32_t()> readableCallBack{};
		function<void()> readableNotification{};

		MsgWebSocketSSLClient(string, string);

		void writeData(vector<uint8_t>&);

		void readData(vector<uint8_t>&);

		uint64_t getBytesOut();

		uint64_t getBytesIn();

		~MsgWebSocketSSLClient();

	protected:
				
		uint32_t fileDescriptor{ static_cast<uint32_t>(~0) };
		const uint32_t bufferSize{ 1024 * 16 };
		vector<uint8_t> outputBuffer{};
		vector<uint8_t> inputBuffer{};
		MsgWebSocketSSLContext ssl{};
		uint64_t bytesOut{ 0 };
		uint64_t lastTime{ 0 };
		uint64_t bytesIn{ 0 };
		string hostname{ "" };
		bool blocking{ true };
		string cipher{ "" };
		string port{ "" };
	};

}