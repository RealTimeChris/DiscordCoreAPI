/*
*    # License

   Copyright 2021 Chris M.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

	   http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
/// SSLClents.hpp - Header file for the "SSL Client" stuff.
/// Dec 12, 2021
/// Chris M.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#ifndef OPENSSL_NO_DEPRECATED
#define OPENSSL_NO_DEPRECATED
#endif

#ifdef _WIN32
#pragma comment(lib, "libcrypto.lib")
#pragma comment(lib, "libssl.lib")
#pragma comment(lib, "ws2_32")
#include <WinSock2.h>
#include <WS2tcpip.h>
#else
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <netdb.h>
#endif

#include <FoundationEntities.hpp>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <algorithm>
#include <iostream>

namespace DiscordCoreInternal {

	using SOCKET = uint64_t;

#ifndef INVALID_SOCKET
#define INVALID_SOCKET	static_cast<SOCKET>(~0)
#endif
#ifndef SOCKET_ERROR
#define SOCKET_ERROR	(-1)
#endif

	DiscordCoreAPI_Dll void reportError(std::string errorPosition, int32_t errorValue);

#ifndef _WIN32
	struct DiscordCoreAPI_Dll epollWrapper {

		struct DiscordCoreAPI_Dll epollDeleter {
			void operator()(int32_t* other) {
				if (auto resultValue = close(*other); resultValue == SOCKET_ERROR) {
					reportError("close() Error: ", resultValue);
				}
			}
		};

		operator int32_t() {
			return *this->thePtr;
		}

		epollWrapper(nullptr_t) {
			*this->thePtr = epoll_create1(0);
			if (*this->thePtr == SOCKET_ERROR) {
				reportError("epoll_create1() Error: ", *this->thePtr);
			}
		};

	protected:
		std::unique_ptr<int32_t, epollDeleter> thePtr{ new int32_t{-1}, epollDeleter{} };
	};

#endif
	struct DiscordCoreAPI_Dll BIOWrapper {

		struct DiscordCoreAPI_Dll BIODeleter {
			void operator()(BIO* other) {
				if (other != nullptr) {
					BIO_free(other);
					other = nullptr;
				}
			}
		};

		BIOWrapper& operator=(BIO* other) {
			this->bioPtr.reset(other);
			if (!BIO_up_ref(other)) {
				std::cout << "BIO_up_ref() Error: ";
				ERR_print_errors_fp(stdout);
				std::cout << std::endl;
			};
			return *this;
		}

		operator BIO*() {
			return this->bioPtr.get();
		}

		BIOWrapper(nullptr_t) {};

	protected:
		std::unique_ptr<BIO, BIODeleter> bioPtr{ nullptr, BIODeleter{} };
	};

	struct DiscordCoreAPI_Dll addrinfoWrapper {

		struct DiscordCoreAPI_Dll addrinfoDeleter {
			void operator()(addrinfo* other) {
				if (other != nullptr) {
					freeaddrinfo(other);
					other = nullptr;
				}
			}
		};

		addrinfo* operator->() {
			return this->addrinfoPtrTwo;
		}

		operator addrinfo**() {
			return &this->addrinfoPtrTwo;
		}

		operator addrinfo*() {
			return this->addrinfoPtrTwo;
		}

		addrinfoWrapper(nullptr_t) {
			this->addrinfoPtrTwo = this->addrinfoPtr.get();
		};

	protected:
		std::unique_ptr<addrinfo, addrinfoDeleter> addrinfoPtr{ new addrinfo{}, addrinfoDeleter{} };
		addrinfo* addrinfoPtrTwo{ nullptr };
	};

	struct DiscordCoreAPI_Dll SSL_CTXWrapper {

		struct DiscordCoreAPI_Dll SSL_CTXDeleter {
			void operator()(SSL_CTX* other) {
				if (other != nullptr) {
					SSL_CTX_free(other);
					other = nullptr;
				}
			}
		};

		SSL_CTXWrapper& operator=(SSL_CTX* other) {
			this->sslCTXPtr.reset(other);
			if (!SSL_CTX_up_ref(other)) {
				std::cout << "SSL_CTX_up_ref() Error: ";
				ERR_print_errors_fp(stdout);
				std::cout << std::endl;
			}
			return *this;
		}

		operator SSL_CTX*() {
			return this->sslCTXPtr.get();
		}

		SSL_CTXWrapper(nullptr_t) {};

	protected:
		std::unique_ptr<SSL_CTX, SSL_CTXDeleter> sslCTXPtr{ nullptr , SSL_CTXDeleter{} };
	};

	struct DiscordCoreAPI_Dll SSLWrapper {

		struct DiscordCoreAPI_Dll SSLDeleter {
			void operator()(SSL* other) {
				if (other != nullptr) {
					SSL_shutdown(other);
					SSL_free(other);
					other = nullptr;
				}
			}
		};

		SSLWrapper& operator=(SSL* other) {
			this->sslPtr.reset(other);
			if (!SSL_up_ref(other)) {
				std::cout << "SSL_up_ref() Error: ";
				ERR_print_errors_fp(stdout);
				std::cout << std::endl;
			}
			return *this;
		}

		operator SSL*() {
			return this->sslPtr.get();
		}

		SSLWrapper(nullptr_t) {};

	protected:
		std::unique_ptr<SSL, SSLDeleter> sslPtr{ nullptr , SSLDeleter{} };
	};

	struct DiscordCoreAPI_Dll SOCKETWrapper {

		struct DiscordCoreAPI_Dll SOCKETDeleter {
			void operator()(SOCKET* other) {
				if (other != nullptr) {
#ifdef _WIN32
					shutdown(*other, 2);
					closesocket(*other);
#else
					close(*other);
#endif
					other = nullptr;
				}
			}
		};

		SOCKETWrapper& operator=(SOCKET other) {
			*this->socketPtr = other;
			return *this;
		}

		operator SOCKET() {
			return *this->socketPtr;
		}

		SOCKETWrapper(nullptr_t) {}

	protected:
		std::unique_ptr<SOCKET, SOCKETDeleter> socketPtr{ new SOCKET{INVALID_SOCKET}, SOCKETDeleter{} };
	};

	struct X509Deleter {
		void operator()(X509* other) {
			X509_free(other);
		}
	};

	class DiscordCoreAPI_Dll HttpSSLClient {
	public:

		HttpSSLClient(std::string* theVector);

		HttpSSLClient(nullptr_t);

		bool connect(std::string baseUrl, std::string portNew = "443");

		void writeData(std::string& theData);

		std::string& getInputBuffer();

		bool processIO();

		virtual ~HttpSSLClient() = default;

	protected:

		BIOWrapper connectionBio{ nullptr };
		std::vector<uint8_t> outputBuffer{};
		int32_t maxBufferSize{ 16 * 1024 };
		SOCKETWrapper theSocket{ nullptr };
		SSL_CTXWrapper context{ nullptr };
		std::string soundcloudCertPath{};
		std::string defaultCertPath{};
		std::string googleCertPath{};
		std::string inputBuffer{};
		SSLWrapper ssl{ nullptr };
		bool wantRead{ false };
	};

	template <typename ObjectType>
	concept StringOrVector = requires(ObjectType v)
	{
		{v.data() }->std::convertible_to<char*>;
	} || requires(ObjectType v)
	{
		{v.data()}->std::convertible_to<uint8_t*>;
	};

	class DiscordCoreAPI_Dll WebSocketSSLClient {
	public:

		WebSocketSSLClient(std::string baseUrl, std::string port, int64_t maxBufferSize = 16 * 1024);

		WebSocketSSLClient(nullptr_t);

		template<StringOrVector TypeName>
		void writeData(TypeName& data) {
			this->outputBuffer.insert(this->outputBuffer.end(), data.begin(), data.end());
		}

		std::vector<uint8_t>& getInputBuffer();

		int64_t getBytesRead();

		std::string getData();

		bool processIO(int32_t waitTimeInMicroSeconds);

	protected:

		const int32_t maxBufferSize{ 1024 * 16 };
		std::vector<uint8_t> outputBuffer{};
		SOCKETWrapper theSocket{ nullptr };
		std::vector<uint8_t> inputBuffer{};
		SSL_CTXWrapper context{ nullptr };
		SSLWrapper ssl{ nullptr };
		bool wantRead{ false };
		int64_t bytesRead{ 0 };
	};

	class DiscordCoreAPI_Dll DatagramSocketSSLClient {
	public:

		DatagramSocketSSLClient(std::string hostName, std::string port);

		DatagramSocketSSLClient(nullptr_t);

		bool writeData(std::vector<uint8_t>& dataToWrite);

		std::vector<uint8_t>& getInputBuffer();

		std::vector<uint8_t> getData();

		void readData(bool doWeClear);

	protected:

		const int32_t maxBufferSize{ 1024 * 16 };
		SOCKETWrapper theSocket{ nullptr };
		std::vector<uint8_t> inputBuffer{};
		BIOWrapper datagramBio{ nullptr };
	};
}
