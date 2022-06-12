/*
*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

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

#ifdef max
	#undef max
#endif

#ifdef min
	#undef min
#endif

namespace DiscordCoreInternal {

	using SOCKET = int32_t;

#ifndef SOCKET_ERROR
	#define SOCKET_ERROR (-1)
#endif

	struct ConnectionError : public std::runtime_error {
		explicit ConnectionError(const std::string& theString) : std::runtime_error(theString){};
	};

	struct ProcessingError : public std::runtime_error {
		explicit ProcessingError(const std::string& theString) : std::runtime_error(theString){};
	};

	DiscordCoreAPI_Dll std::string reportError(const std::string& errorPosition, int32_t errorValue) noexcept;
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
				std::cout << DiscordCoreAPI::reset() << std::endl;
			}
		}

	  protected:
		std::unique_ptr<WSADATA, WSADataDeleter> thePtr{ new WSADATA{}, WSADataDeleter{} };
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
				std::cout << DiscordCoreAPI::reset() << std::endl;
			};
			return *this;
		}

		operator BIO*() {
			return this->bioPtr.get();
		}

		BIOWrapper(std::nullptr_t){};

	  protected:
		std::unique_ptr<BIO, BIODeleter> bioPtr{ nullptr, BIODeleter{} };
	};

	struct DiscordCoreAPI_Dll addrinfoWrapper {
		addrinfo* operator->() {
			if (this->addrinfoPtrTwo == nullptr) {
				throw ConnectionError{ "Error: addrinfoPtrTwo was nullptr." };
			}
			return this->addrinfoPtrTwo;
		}

		operator addrinfo**() {
			this->doWeClearAddrInfo = true;
			if (this->addrinfoPtrTwo == nullptr) {
				throw ConnectionError{ "Error: addrinfoPtrTwo was nullptr." };
			}
			return &this->addrinfoPtrTwo;
		}

		operator addrinfo*() {
			if (this->addrinfoPtrTwo == nullptr) {
				throw ConnectionError{ "Error: addrinfoPtrTwo was nullptr." };
			}
			return this->addrinfoPtrTwo;
		}

		addrinfoWrapper(std::nullptr_t){};

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
				std::cout << DiscordCoreAPI::reset() << std::endl;
			}
			return *this;
		}

		operator SSL_CTX*() {
			return this->sslCTXPtr.get();
		}

		SSL_CTXWrapper(std::nullptr_t){};

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
				std::cout << DiscordCoreAPI::reset() << std::endl;
			}
			return *this;
		}

		operator SSL*() {
			return this->sslPtr.get();
		}

		SSLWrapper(std::nullptr_t){};

	  protected:
		std::unique_ptr<SSL, SSLDeleter> sslPtr{ nullptr, SSLDeleter{} };
	};

	struct SOCKETWrapper {
		struct SOCKETDeleter {
			void operator()(SOCKET* other) {
				if (other) {
#ifdef _WIN32
					shutdown(*other, SD_BOTH);
					closesocket(*other);
#else
					shutdown(*other, SHUT_RDWR);
					close(*other);
#endif
					*other = -1;
				}
			}
		};

		SOCKETWrapper& operator=(SOCKETWrapper&& other) noexcept {
			if (this != &other) {
				this->socketPtr.swap(other.socketPtr);
				*other.socketPtr = SOCKET_ERROR;
				return *this;
			}
		}

		SOCKETWrapper(SOCKETWrapper&& other) noexcept {
			*this = std::move(other);
		}

		SOCKETWrapper& operator=(SOCKET other) {
			*this->socketPtr = other;
			return *this;
		}

		operator SOCKET() {
			return *this->socketPtr;
		}

		SOCKETWrapper(std::nullptr_t) {
		}

	  protected:
		std::unique_ptr<SOCKET, SOCKETDeleter> socketPtr{ new SOCKET{ static_cast<SOCKET>(SOCKET_ERROR) }, SOCKETDeleter{} };
	};

	struct DiscordCoreAPI_Dll X509Deleter {
		void operator()(X509* other) {
			X509_free(other);
		}
	};

	class DiscordCoreAPI_Dll HttpSSLClient {
	  public:
		HttpSSLClient() = default;

		void connect(const std::string& baseUrl, const std::string& portNew = "443");

		void writeData(std::string& data) noexcept;

		std::string& getInputBuffer() noexcept;

		void processIO();

		virtual ~HttpSSLClient() noexcept = default;

	  protected:
		static std::string soundcloudCertPathStatic;
		static std::string defaultCertPathStatic;
		static std::string googleCertPathStatic;
		static std::mutex theMutex;

		int32_t maxBufferSize{ (1024 * 16) - 1 };
		std::vector<std::string> outputBuffer{};
		BIOWrapper connectionBio{ nullptr };
		SOCKETWrapper theSocket{ nullptr };
		SSL_CTXWrapper context{ nullptr };
		std::string inputBuffer{};
		SSLWrapper ssl{ nullptr };
		bool wantWrite{ false };
		bool wantRead{ false };
	};

	class DiscordCoreAPI_Dll WebSocketSSLShard {
	  public:
		friend class VoiceSocketAgent;
		friend class BaseSocketAgent;

		WebSocketSSLShard(int32_t maxBufferSizeNew, int32_t currentShard, int32_t totalShards) noexcept;

		WebSocketSSLShard() = default;

		void connect(const std::string& baseUrlNew, const std::string& portNew);

		bool processIO(std::unordered_map<SOCKET, std::unique_ptr<WebSocketSSLShard>>& theMap) noexcept;

		void writeData(std::string& data) noexcept;

		std::string getInputBuffer() noexcept;

		int64_t getBytesRead() noexcept;

		~WebSocketSSLShard() noexcept = default;

	  protected:
		int32_t maxBufferSize{ (1024 * 16) - 1 };
		std::vector<std::string> outputBuffer{};
		bool haveWeReceivedHeartbeatAck{ true };
		SOCKETWrapper theSocket{ nullptr };
		SSL_CTXWrapper context{ nullptr };
		bool areWeHeartBeating{ false };
		int32_t lastNumberReceived{ 0 };
		bool areWeConnected{ false };
		SSLWrapper ssl{ nullptr };
		std::string inputBuffer{};
		nlohmann::json shard{};
		bool wantWrite{ true };
		bool wantRead{ false };
		int64_t bytesRead{ 0 };
	};

	class DiscordCoreAPI_Dll DatagramSocketSSLClient {
	  public:
		DatagramSocketSSLClient() noexcept = default;

		void connect(const std::string& hostName, const std::string& port);

		void writeData(const std::string& data);

		std::string& getInputBuffer() noexcept;

		bool readData(bool doWeClear) noexcept;

		int64_t getBytesRead() noexcept;

		~DatagramSocketSSLClient() noexcept = default;

	  protected:
		const int32_t maxBufferSize{ 1024 * 16 };
		SOCKETWrapper theSocket{ nullptr };
		BIOWrapper datagramBio{ nullptr };
		std::string inputBuffer{};
		int64_t bytesRead{ 0 };
	};
}// namespace DiscordCoreInterna