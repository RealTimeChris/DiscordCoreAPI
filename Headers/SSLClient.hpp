/// SSLClent.hpp - Header file for the "SSL Client" stuff.
/// Dec 12, 2021
/// Chris M.
/// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"
#include <stdio.h>

namespace DiscordCoreInternal {

	struct SSL_METHODDeleter {
		void operator()(const SSL_METHOD*) {};
	};

	struct addrinfoDeleter {
		void operator()(addrinfo* other) {
			freeaddrinfo(other);
		}
	};

	class Counter {
	public:

		void incrementCount() {
			this->count += 1;
		}

		void decrementCount() {
			this->count -= 1;
		}

		uint32_t getCount() {
			return this->count;
		}
	protected:
		uint32_t count{ 0 };
	};

	struct addrinfoWrapper {

		addrinfoWrapper(nullptr_t) {
			ZeroMemory(this->thePtr.get(), sizeof(addrinfo));
		}

		addrinfo* operator->() {
			return this->thePtr.get();
		}

		operator PADDRINFOA() {
			return this->thePtr.get();
		}

		addrinfoWrapper& operator=(PADDRINFOA& other) {
			this->thePtr.reset(other);
			return *this;
		}

		addrinfoWrapper(PADDRINFOA& other) {
			*this->thePtr = *other;
		}

		addrinfoWrapper(addrinfoWrapper& other) {
			this->thePtr.swap(other.thePtr);
		}

		addrinfo* operator=(addrinfoWrapper& other) {
			this->thePtr.swap(other.thePtr);
			return other.thePtr.get();
		}
		unique_ptr<addrinfo, addrinfoDeleter> thePtr{ new addrinfo{}, addrinfoDeleter{} };
	};

	struct SSL_CTXDeleter {
		void operator()(SSL_CTX* other) {
			if (other != nullptr) {
				SSL_CTX_free(other);
				other = nullptr;
			}
		}
	};

	struct SSL_CTXWrapper {

		SSL_CTXWrapper(nullptr_t) {};

		SSL_CTXWrapper& operator=(SSL_CTX* other) {
			this->thePtr = unique_ptr<SSL_CTX, SSL_CTXDeleter>(other, SSL_CTXDeleter{});
			if (SSL_CTX_up_ref(other) != 1) {
				cout << "SSL_CTX_up_ref() Error: " << ERR_get_error() << endl;
			}
			return *this;
		}

		operator SSL_CTX* () {
			return this->thePtr.get();
		}

		~SSL_CTXWrapper() {}

	protected:
		unique_ptr<SSL_CTX, SSL_CTXDeleter> thePtr{ nullptr , SSL_CTXDeleter{} };
	};

	struct SSLDeleter {
		void operator()(SSL* other) {
			if (other != nullptr) {
				SSL_free(other);
				other = nullptr;
			}
		}
	};

	struct SSLWrapper {

		SSLWrapper(nullptr_t) {};

		SSLWrapper& operator=(SSL* other) {
			this->thePtr = unique_ptr<SSL, SSLDeleter>(other, SSLDeleter{});
			if (SSL_up_ref(other) != 1) {
				cout << "SSL_up_ref() Error: " << ERR_get_error() << endl;
			}
			return *this;
		}

		operator SSL* () {
			return this->thePtr.get();
		}

		~SSLWrapper() {}

		protected:
			unique_ptr<SSL, SSLDeleter> thePtr{ nullptr , SSLDeleter{} };
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
		void operator()(WSADATA*) {
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

		unique_ptr<Socket, SocketDeleter> fileDescriptor{ new Socket(), SocketDeleter{} };
		const uint32_t bufferSize{ 1024 * 16 };
		SSL_CTXWrapper context{ nullptr };
		vector<uint8_t> inputBuffer{};
		bool areWeBlocking{ true };
		SSLWrapper ssl{ nullptr };
		string hostname{ "" };
		string port{ "" };
		fd_set readfds{};
	};

	class DiscordCoreAPI_Dll DatagramWebSocketSSLClient {
	public:

		DatagramWebSocketSSLClient(string hostName, string post);

		DatagramWebSocketSSLClient(nullptr_t);

		void writeData(string& dataToWrite);

		vector<char>& getInputBuffer();

		bool readData(bool doWeClear);

		vector<uint8_t> getData();

		int64_t getBytesWritten();

		int64_t getBytesRead();

		void toggleBlocking();

		~DatagramWebSocketSSLClient();

	protected:

		unique_ptr<Socket, SocketDeleter> fileDescriptor{ new Socket(), SocketDeleter{} };
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

		unique_ptr<Socket, SocketDeleter> fileDescriptor{ new Socket(), SocketDeleter{} };
		SSL_CTXWrapper  context{ nullptr };
		uint64_t bufferSize{ 1024 * 16 };
		vector<char> inputBuffer{};
		bool areWeBlocking{ true };
		int64_t bytesWritten{ 0 };
		SSLWrapper ssl{ nullptr };
		int64_t bytesRead{ 0 };
		string hostname{ "" };
		string port{ "" };
		fd_set readfds{};
	};

}