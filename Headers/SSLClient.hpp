/// SSLClent.hpp - Header file for the "SSL Client" stuff.
/// Dec 12, 2021
/// Chris M.
/// https://github.com/RealTimeChris

#pragma once

#include "IndexInitial.hpp"
#include "FoundationEntities.hpp"
#include "EventEntities.hpp"

namespace DiscordCoreInternal {

	struct DiscordCoreAPI_Dll BIOWrapper {

		struct BIODeleter {
			void operator()(BIO* other) {
				if (other != nullptr) {
					BIO_free(other);
					other = nullptr;
				}
			}
		};

		BIOWrapper& operator=(BIO*other) {
			this->thePtr.reset(other);
			if (BIO_up_ref(other) != 1) {
				cout << "BIO_up_ref() Error: " << ERR_get_error() << endl;
			};
			return *this;
		}

		operator BIO*() {
			return this->thePtr.get();
		}

		BIOWrapper(nullptr_t) {};

	protected:
		unique_ptr<BIO, BIODeleter> thePtr{ nullptr, BIODeleter{} };
	};

	struct DiscordCoreAPI_Dll SSL_METHODWrapper {

		struct DiscordCoreAPI_Dll SSL_METHODDeleter {
			void operator()(const SSL_METHOD*) {}
		};

		SSL_METHODWrapper& operator=(const SSL_METHOD* other) {
			this->thePtr.reset(other);
			return *this;
		}

		operator const SSL_METHOD*() {
			return this->thePtr.get();
		}

		SSL_METHODWrapper(nullptr_t) {};

		SSL_METHODWrapper(const SSL_METHOD* other) {
			*this = other;
		}

	protected:
		unique_ptr<const SSL_METHOD, SSL_METHODDeleter> thePtr{ nullptr, SSL_METHODDeleter{} };
	};

	struct DiscordCoreAPI_Dll addrinfoWrapper {

		struct addrinfoDeleter {
			void operator()(addrinfo* other) {
				if (other != nullptr) {
					freeaddrinfo(other);
					other = nullptr;
				}
			}
		};

		addrinfoWrapper& operator=(addrinfo* other) {
			this->thePtr.reset(other);
			return *this;
		}

		addrinfo* operator->() {
			return this->thePtr.get();
		}

		operator addrinfo*() {
			return this->thePtr.get();
		}

		addrinfoWrapper(nullptr_t) {
			ZeroMemory(this->thePtr.get(), sizeof(addrinfo));
		};

	protected:
		unique_ptr<addrinfo, addrinfoDeleter> thePtr{ new addrinfo, addrinfoDeleter{} };
	};

	struct DiscordCoreAPI_Dll SSL_CTXWrapper {

		struct SSL_CTXDeleter {
			void operator()(SSL_CTX* other) {
				if (other != nullptr) {
					SSL_CTX_free(other);
					other = nullptr;
				}
			}
		};

		SSL_CTXWrapper& operator=(SSL_CTX* other) {
			this->thePtr.reset(other);
			if (SSL_CTX_up_ref(other) != 1) {
				cout << "SSL_CTX_up_ref() Error: " << ERR_get_error() << endl;
			}
			return *this;
		}

		operator SSL_CTX*() {
			return this->thePtr.get();
		}

		SSL_CTXWrapper(nullptr_t) {};

	protected:
		unique_ptr<SSL_CTX, SSL_CTXDeleter> thePtr{ nullptr , SSL_CTXDeleter{} };
	};

	struct DiscordCoreAPI_Dll SSLWrapper {

		struct SSLDeleter {
			void operator()(SSL* other) {
				if (other != nullptr) {
					SSL_free(other);
					other = nullptr;
				}
			}
		};

		SSLWrapper& operator=(SSL* other) {
			this->thePtr.reset(other);
			if (SSL_up_ref(other) != 1) {
				cout << "SSL_up_ref() Error: " << ERR_get_error() << endl;
			}
			return *this;
		}

		operator SSL*() {
			return this->thePtr.get();
		}

		SSLWrapper(nullptr_t) {};

	protected:
		unique_ptr<SSL, SSLDeleter> thePtr{ nullptr , SSLDeleter{} };
	};

	struct DiscordCoreAPI_Dll SOCKETWrapper {

		struct SOCKETDeleter {
			void operator()(SOCKET* other) {
#ifdef _WIN32
				shutdown(*other, 2);
				closesocket(*other);
#else
				close(*other);
#endif
			}
		};

		SOCKETWrapper& operator=(SOCKET other) {
			*this->thePtr = other;
			return *this;
		}

		operator SOCKET() {
			return *this->thePtr;
		}

		SOCKETWrapper(nullptr_t) {}

	protected:

		unique_ptr<SOCKET, SOCKETDeleter>thePtr{ new SOCKET{}, SOCKETDeleter{} };
	};

	struct DiscordCoreAPI_Dll WSADATAWrapper {

		struct WSADATADeleter {
			void operator()(WSADATA*) {
				WSACleanup();
			}
		};

		WSADATAWrapper() {
			int32_t errorCode = WSAStartup(MAKEWORD(2, 2), this->thePtr.get());
			if (errorCode != 0) {
				cout << "WSAStartup Error: " << errorCode << endl;
			};
		}

	protected:
		unique_ptr<WSADATA, WSADATADeleter>thePtr{ new WSADATA{}, WSADATADeleter{} };
	};

	class DiscordCoreAPI_Dll MsgWebSocketSSLClient {
	public:

		MsgWebSocketSSLClient(string, string);

		MsgWebSocketSSLClient(nullptr_t);

		void writeData(vector<uint8_t>&);

		vector<uint8_t>& getInputBuffer();

		void writeData(string&);

		bool readData();

		~MsgWebSocketSSLClient();

	protected:
		
		DiscordCoreAPI::Event<void, void> finishEvent{};
		SOCKETWrapper fileDescriptor{ nullptr };
		const uint32_t bufferSize{ 1024 * 16 };
		SSL_CTXWrapper context{ nullptr };
		vector<uint8_t> inputBuffer{};
		SSLWrapper ssl{ nullptr };
		bool doWeQuit{ false };
		string hostname{ "" };
		string port{ "" };
		fd_set readSet{};
	};

	class DiscordCoreAPI_Dll DatagramWebSocketSSLClient {
	public:

		DatagramWebSocketSSLClient(string hostName, string post);

		DatagramWebSocketSSLClient(nullptr_t);

		void writeData(string& dataToWrite);

		vector<char>& getInputBuffer();

		bool readData(bool doWeClear);

		vector<uint8_t> getData();

		void toggleBlocking();

		~DatagramWebSocketSSLClient();

	protected:

		SOCKETWrapper fileDescriptor{ nullptr };
		const uint32_t bufferSize{ 1024 * 16 };
		vector<char> inputBuffer{};
		bool areWeBlocking{ true };
		bool doWeQuit{ false };
		string hostname{ "" };
		string port{ "" };
	};

	class DiscordCoreAPI_Dll StreamWebSocketSSLClient {
	public:

		StreamWebSocketSSLClient(string hostName, string post, uint64_t bufferSize);

		StreamWebSocketSSLClient(nullptr_t);

		void writeData(vector<uint8_t>& dataToWrite);

		vector<uint8_t> getData();

		int64_t getBytesRead();

		bool readData();

	protected:

		SOCKETWrapper fileDescriptor{ nullptr };
		SSL_CTXWrapper  context{ nullptr };
		vector<char> inputBuffer{};
		SSLWrapper ssl{ nullptr };
		uint64_t bufferSize{ 0 };
		int64_t bytesRead{ 0 };
		string hostname{ "" };
		string port{ "" };
		fd_set readSet{};
	};

}