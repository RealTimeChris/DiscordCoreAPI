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
		void operator()(const SSL_METHOD* other) {}
	};

	struct SSL_METHODWrapper {

		SSL_METHODWrapper(nullptr_t) {};

		SSL_METHODWrapper& operator=(const SSL_METHOD* other) {
			this->thePtr = unique_ptr<const SSL_METHOD, SSL_METHODDeleter>(other, SSL_METHODDeleter{});
			return *this;
		}

		operator const SSL_METHOD* () {
			return this->thePtr.get();
		}

	protected:
		unique_ptr<const SSL_METHOD, SSL_METHODDeleter> thePtr{ nullptr, SSL_METHODDeleter{} };
	};

	struct addrinfoDeleter {
		void operator()(addrinfo* other) {
			if (other != nullptr) {
				freeaddrinfo(other);
				other = nullptr;
			}
		}
	};

	struct addrinfoWrapper {

		addrinfoWrapper(nullptr_t) {};

		addrinfoWrapper& operator=(addrinfo* other) {
			this->thePtr = unique_ptr<addrinfo, addrinfoDeleter>(other, addrinfoDeleter{});
			return *this;
		}

		addrinfo* operator->() {
			return this->thePtr.get();
		}

		operator addrinfo* () {
			return this->thePtr.get();
		}

	protected:
		unique_ptr<addrinfo, addrinfoDeleter> thePtr{ nullptr , addrinfoDeleter{} };
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

		protected:
			unique_ptr<SSL, SSLDeleter> thePtr{ nullptr , SSLDeleter{} };
	};

	struct Socket {
		uint64_t theSocket{ 0 };
	};

	struct SOCKETDeleter {
		void operator()(SOCKET* other) {
#ifdef _WIN32
			shutdown(*other, 2);
			closesocket(*other);
#else
			close(other->theSocket);
#endif
		}
	};

	struct SOCKETWrapper {

		SOCKETWrapper(nullptr_t){}

		SOCKETWrapper(SOCKET other) {
			this->thePtr = unique_ptr<SOCKET, SOCKETDeleter>(&other, SOCKETDeleter{});
		}

		operator SOCKET() {
			return *this->thePtr;
		}

		SOCKETWrapper& operator=(SOCKET other) {
			this->thePtr.reset(&other);
			return *this;
		}		

		SOCKET operator=(SOCKETWrapper& other) {
			this->thePtr.swap(other.thePtr);
			return *this->thePtr.get();
		}

		unique_ptr<SOCKET, SOCKETDeleter> thePtr{ nullptr };
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

		SOCKETWrapper fileDescriptor{ nullptr };
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

		SOCKETWrapper fileDescriptor{ nullptr };
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

		SOCKETWrapper fileDescriptor{ nullptr };
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