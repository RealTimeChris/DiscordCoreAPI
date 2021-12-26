/// SSLClent.hpp - Header file for the "SSL Client" stuff.
/// Dec 12, 2021
/// Chris M.
/// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"

namespace DiscordCoreInternal {

	struct SSL_CTXDeleter {
		void operator()(SSL_CTX* other) {
			SSL_CTX_free(other);
		}
	};

	struct SSLDeleter {
		void operator()(SSL* other) {
			SSL_shutdown(other);
		}
	};

	struct Socket {
		uint64_t theSocket{ 0 };
	};

	struct SocketDeleter {
		void operator()(Socket* other) {
			shutdown(other->theSocket, 2);
			closesocket(other->theSocket);
		}
	};

	class DiscordCoreAPI_Dll MsgWebSocketSSLClient {
	public:

		friend class VoiceChannelWebSocketAgent;
		friend class VoiceChannelWebSocketAgent;
		friend class MsgWebSocketAgent;
		
		MsgWebSocketSSLClient(string, string);

		MsgWebSocketSSLClient(nullptr_t);

		void writeData(vector<uint8_t>&);

		void writeData(string&);
		
		void toggleBlocking();

		bool readData();

		~MsgWebSocketSSLClient();

	protected:

		unique_ptr<SSL_CTX, SSL_CTXDeleter> context{ nullptr };
		unique_ptr<Socket> fileDescriptor{ nullptr };
		unique_ptr<SSL, SSLDeleter> ssl{ nullptr };
		const uint32_t bufferSize{ 1024 * 16 };
		vector<uint8_t> inputBuffer{};
		bool areWeBlocking{ true };
		string hostname{ "" };
		string port{ "" };
		fd_set readfds{};
	};

	class DiscordCoreAPI_Dll DatagramWebSocketSSLClient {
	public:

		friend class VoiceChannelWebSocketAgent;

		DatagramWebSocketSSLClient(string hostName, string post);

		string collectExternalIp(uint32_t audioSSRC);

		void writeData(string& dataToWrite);

		DatagramWebSocketSSLClient(nullptr_t);

		bool readData(bool doWeClear);

		int64_t getBytesWritten();

		vector<uint8_t> getData();

		int64_t getBytesRead();

		void toggleBlocking();

		~DatagramWebSocketSSLClient();

	protected:

		unique_ptr<Socket> fileDescriptor{ nullptr };
		const uint32_t bufferSize{ 1024 * 16 };
		vector<char> inputBuffer{};
		bool areWeBlocking{ true };
		int64_t bytesWritten{ 0 };
		int64_t bytesRead{ 0 };
		string hostname{ "" };
		string port{ "" };
	};

	class DiscordCoreAPI_Dll StreamWebSocketSSLClient {
	public:

		friend class VoiceChannelWebSocketAgent;

		StreamWebSocketSSLClient(string hostName, string post, uint64_t bufferSize);

		void writeData(vector<uint8_t>& dataToWrite);

		StreamWebSocketSSLClient(nullptr_t);

		bool readData();

		int64_t getBytesWritten();

		vector<uint8_t> getData();

		int64_t getBytesRead();

		void toggleBlocking();

		~StreamWebSocketSSLClient();

	protected:

		unique_ptr<SSL_CTX, SSL_CTXDeleter> context{ nullptr };
		unique_ptr<Socket> fileDescriptor{ nullptr };
		unique_ptr<SSL, SSLDeleter> ssl{ nullptr };
		uint64_t bufferSize{ 1024 * 16 };
		vector<char> inputBuffer{};
		bool areWeBlocking{ true };
		int64_t bytesWritten{ 0 };
		int64_t bytesRead{ 0 };
		string hostname{ "" };
		string port{ "" };
		fd_set readfds{};
	};

}