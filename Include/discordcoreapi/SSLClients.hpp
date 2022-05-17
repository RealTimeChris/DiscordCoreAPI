/*
*
	Copyright 2021, 2022 Chris M. (RealTimeChris)

	This file is part of DiscordCoreAPI.
	DiscordCoreAPI is free software: you can redistribute it and/or modify it under the terms of the GNU
	General Public License as published by the Free Software Foundation, either version 3 of the License,
	or (at your option) any later version.
	DiscordCoreAPI is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
	even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
	You should have received a copy of the GNU General Public License along with DiscordCoreAPI.
	If not, see <https://www.gnu.org/licenses/>.

*/
/// SSLClents.hpp - Header file for the "SSL Client" stuff.
/// Dec 12, 2021
/// Chris M.
/// https://discordcoreapi.com
/// \file SSLClients.hpp

#pragma once

#ifndef OPENSSL_NO_DEPRECATED
	#define OPENSSL_NO_DEPRECATED
#endif

#ifdef _WIN32
	#pragma comment(lib, "Ws2_32.lib")
	#include <WS2tcpip.h>
	#include <WinSock2.h>
#else
	#include <fcntl.h>
	#include <netdb.h>
	#include <netinet/in.h>
	#include <netinet/tcp.h>
	#include <stdint.h>
	#include <sys/epoll.h>
	#include <sys/socket.h>
	#include <sys/types.h>
	#include <unistd.h>
#endif

#include <algorithm>
#include <filesystem>
#include <discordcoreapi/FoundationEntities.hpp>
#include <openssl/err.h>
#include <openssl/ssl.h>

namespace DiscordCoreInternal {

	using SOCKET = uint64_t;

#ifndef INVALID_SOCKET
	#define INVALID_SOCKET static_cast<SOCKET>(~0)
#endif
#ifndef SOCKET_ERROR
	#define SOCKET_ERROR (-1)
#endif

	DiscordCoreAPI_Dll void reportError(const std::string& errorPosition, int32_t errorValue) noexcept;
#ifdef _WIN32
	struct DiscordCoreAPI_Dll WSADataWrapper {
		struct DiscordCoreAPI_Dll WSADataDeleter {
			void operator()(WSADATA* other) {
				WSACleanup();
				delete other;
			}
		};

		WSADataWrapper() {
			if (auto errorValue = WSAStartup(MAKEWORD(2, 2), this->thePtr.get()); errorValue != 0) {
				std::cout << DiscordCoreAPI::shiftToBrightRed() << "WSAStartup() Error: " << errorValue << ", ";
				std::cout << std::endl << DiscordCoreAPI::reset();
			}
		}

	  protected:
		std::unique_ptr<WSADATA, WSADataDeleter> thePtr{ new WSADATA{}, WSADataDeleter{} };
	};
#endif
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
		std::unique_ptr<int32_t, epollDeleter> thePtr{ new int32_t{ -1 }, epollDeleter{} };
	};
#endif

	struct DiscordCoreAPI_Dll BIOWrapper {
		struct DiscordCoreAPI_Dll BIODeleter {
			void operator()(BIO* other) {
				if (other) {
					BIO_free(other);
					other = nullptr;
				}
			}
		};

		BIOWrapper& operator=(BIO* other) {
			this->bioPtr.reset(other);
			if (!BIO_up_ref(other)) {
				std::cout << DiscordCoreAPI::shiftToBrightRed() << "BIO_up_ref() Error: ";
				ERR_print_errors_fp(stdout);
				std::cout << std::endl << DiscordCoreAPI::reset();
			};
			return *this;
		}

		operator BIO*() {
			return this->bioPtr.get();
		}

		BIOWrapper(nullptr_t){};

	  protected:
		std::unique_ptr<BIO, BIODeleter> bioPtr{ nullptr, BIODeleter{} };
	};

	struct DiscordCoreAPI_Dll addrinfoWrapper {
		addrinfo* operator->() {
			return this->addrinfoPtrTwo;
		}

		operator addrinfo**() {
			this->doWeClearAddrInfo = true;
			return &this->addrinfoPtrTwo;
		}

		operator addrinfo*() {
			return this->addrinfoPtrTwo;
		}

		addrinfoWrapper(nullptr_t){};

		~addrinfoWrapper() {
			if (this->doWeClearAddrInfo) {
				freeaddrinfo(this->addrinfoPtrTwo);
			} else {
				delete this->addrinfoPtrTwo;
			}
		}

	  protected:
		addrinfo* addrinfoPtrTwo{ new addrinfo{} };
		bool doWeClearAddrInfo{ false };
	};

	struct DiscordCoreAPI_Dll SSL_CTXWrapper {
		struct DiscordCoreAPI_Dll SSL_CTXDeleter {
			void operator()(SSL_CTX* other) {
				if (other) {
					SSL_CTX_free(other);
					other = nullptr;
				}
			}
		};

		SSL_CTXWrapper& operator=(SSL_CTX* other) {
			this->sslCTXPtr.reset(other);
			if (!SSL_CTX_up_ref(other)) {
				std::cout << DiscordCoreAPI::shiftToBrightRed() << "SSL_CTX_up_ref() Error: ";
				ERR_print_errors_fp(stdout);
				std::cout << std::endl << DiscordCoreAPI::reset();
			}
			return *this;
		}

		operator SSL_CTX*() {
			return this->sslCTXPtr.get();
		}

		SSL_CTXWrapper(nullptr_t){};

	  protected:
		std::unique_ptr<SSL_CTX, SSL_CTXDeleter> sslCTXPtr{ nullptr, SSL_CTXDeleter{} };
	};

	struct DiscordCoreAPI_Dll SSLWrapper {
		struct DiscordCoreAPI_Dll SSLDeleter {
			void operator()(SSL* other) {
				if (other) {
					SSL_shutdown(other);
					SSL_free(other);
					other = nullptr;
				}
			}
		};

		SSLWrapper& operator=(SSL* other) {
			this->sslPtr.reset(other);
			if (!SSL_up_ref(other)) {
				std::cout << DiscordCoreAPI::shiftToBrightRed() << "SSL_up_ref() Error: ";
				ERR_print_errors_fp(stdout);
				std::cout << std::endl << DiscordCoreAPI::reset();
			}
			return *this;
		}

		operator SSL*() {
			return this->sslPtr.get();
		}

		SSLWrapper(nullptr_t){};

	  protected:
		std::unique_ptr<SSL, SSLDeleter> sslPtr{ nullptr, SSLDeleter{} };
	};

	struct DiscordCoreAPI_Dll SOCKETWrapper {
		struct DiscordCoreAPI_Dll SOCKETDeleter {
			void operator()(SOCKET* other) {
				if (other) {
#ifdef _WIN32
					shutdown(*other, SD_BOTH);
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

		SOCKETWrapper(nullptr_t) {
		}

	  protected:
		std::unique_ptr<SOCKET, SOCKETDeleter> socketPtr{ new SOCKET{ INVALID_SOCKET }, SOCKETDeleter{} };
	};

	struct DiscordCoreAPI_Dll X509Deleter {
		void operator()(X509* other) {
			X509_free(other);
		}
	};

	class DiscordCoreAPI_Dll HttpSSLClient {
	  public:
		HttpSSLClient() noexcept = default;

		bool connect(const std::string& baseUrl,  const std::string& portNew = "443") noexcept;

		void writeData(const std::string& theData) noexcept;

		std::string& getInputBuffer() noexcept;

		bool processIO() noexcept;

		static void initialize();

		virtual ~HttpSSLClient() = default;

	  protected:
		static std::string soundcloudCertPathStatic;
		static std::string defaultCertPathStatic;
		static std::string googleCertPathStatic;
		static std::mutex theMutex;

		BIOWrapper connectionBio{ nullptr };
		int32_t maxBufferSize{ 16 * 1024 };
		SOCKETWrapper theSocket{ nullptr };
		SSL_CTXWrapper context{ nullptr };
		std::string outputBuffer{};
		std::string inputBuffer{};
		SSLWrapper ssl{ nullptr };
		bool wantRead{ false };
	};

	class DiscordCoreAPI_Dll WebSocketSSLClient {
	  public:
		WebSocketSSLClient(const std::string& baseUrl,  const std::string& port, int64_t maxBufferSize = 16 * 1024) noexcept;

		WebSocketSSLClient() noexcept = default;

		bool processIO(int32_t waitTimeInMicroSeconds) noexcept;

		void writeData(const std::string& data) noexcept;

		std::string& getInputBuffer() noexcept;

		int64_t getBytesRead() noexcept;

	  protected:
		const int64_t maxBufferSize{ 1024 * 16 };
		SOCKETWrapper theSocket{ nullptr };
		SSL_CTXWrapper context{ nullptr };
		std::string outputBuffer{};
		SSLWrapper ssl{ nullptr };
		std::string inputBuffer{};
		bool wantRead{ false };
		int64_t bytesRead{ 0 };
	};

	class DiscordCoreAPI_Dll DatagramSocketSSLClient {
	  public:
		DatagramSocketSSLClient(const std::string& hostName,  const std::string& port) noexcept;

		DatagramSocketSSLClient() noexcept = default;

		bool writeData(std::string& dataToWrite) noexcept;

		std::string& getInputBuffer() noexcept;

		void readData(bool doWeClear) noexcept;

	  protected:
		const int32_t maxBufferSize{ 1024 * 16 };
		SOCKETWrapper theSocket{ nullptr };
		BIOWrapper datagramBio{ nullptr };
		std::string inputBuffer{};
	};
}// namespace DiscordCoreInternal