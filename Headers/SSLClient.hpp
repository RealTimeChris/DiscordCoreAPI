/// SSLClent.hpp - Header file for the "SSL Client" stuff.
/// Dec 12, 2021
/// Chris M.
/// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"
#include <stdio.h>

namespace DiscordCoreInternal {

	struct SSL_CTXDeleter {
		void operator()(SSL_CTX* other) {
			SSL_CTX_free(other);
		}
	};

	struct SSLDeleter {
		void operator()(SSL* other) {
			SSL_shutdown(other);
			cout << "WERE HERE BEING DELETED!" << endl;
		}
	};

	struct Socket {
		uint64_t theSocket{ 0 };
	};

	struct SocketDeleter {
		void operator()(Socket* other) {
#ifdef _WIN32
			shutdown(other->theSocket, 2);
			closesocket(other->theSocket);
#else
			close(fileDescriptor);
#endif
		}
	};

	struct WSADATADeleter {
		void operator()(WSADATA* other) {
			WSACleanup();
		}
	};

	class DiscordCoreAPI_Dll MsgWebSocketSSLClient {
	public:
		
		MsgWebSocketSSLClient(string, string);

		MsgWebSocketSSLClient(nullptr_t);

		void writeData(vector<uint8_t>&);

		vector<uint8_t>& getInputBuffer();

		void writeData(string&);
		
		void toggleBlocking();

		bool readData();

		~MsgWebSocketSSLClient();

	protected:
		
		unique_ptr<Socket, SocketDeleter> fileDescriptor{ new Socket() ,SocketDeleter() };
		unique_ptr<SSL_CTX, SSL_CTXDeleter> context{ nullptr , SSL_CTXDeleter() };
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

		DatagramWebSocketSSLClient(string hostName, string post);

		DatagramWebSocketSSLClient(nullptr_t);

		string collectExternalIp(uint32_t audioSSRC);

		void writeData(string& dataToWrite);

		bool readData(bool doWeClear);

		vector<uint8_t> getData();
		
		int64_t getBytesWritten();

		int64_t getBytesRead();

		void toggleBlocking();

		~DatagramWebSocketSSLClient();

	protected:

		unique_ptr<Socket, SocketDeleter> fileDescriptor{ new Socket() ,SocketDeleter() };
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

		StreamWebSocketSSLClient(string hostName, string post, uint64_t bufferSize);

		StreamWebSocketSSLClient(nullptr_t);
		
		void writeData(vector<uint8_t>& dataToWrite);

		vector<uint8_t> getData();

		int64_t getBytesWritten();

		int64_t getBytesRead();

		void toggleBlocking();

		bool readData();

		~StreamWebSocketSSLClient();

	protected:

		unique_ptr<Socket, SocketDeleter> fileDescriptor{ new Socket() ,SocketDeleter() };
		unique_ptr<SSL_CTX, SSL_CTXDeleter> context{ nullptr , SSL_CTXDeleter() };
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