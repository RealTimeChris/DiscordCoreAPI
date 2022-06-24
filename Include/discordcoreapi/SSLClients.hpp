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

#ifndef FD_SETSIZE
	#define FD_SETSIZE 400
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

#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/EventEntities.hpp>
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

	DiscordCoreAPI_Dll std::string reportError(const std::string& errorPosition) noexcept;
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
			auto errorValue = BIO_up_ref(other);
			if (!errorValue) {
				std::cout << DiscordCoreAPI::shiftToBrightRed() << "BIO_up_ref() Error: " << ERR_error_string(errorValue, nullptr) << DiscordCoreAPI::reset() << std::endl;
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
				throw ConnectionError{ "addrinfoWrapper::operator->(), addrinfoPtrTwo was nullptr." };
			}
			return this->addrinfoPtrTwo;
		}

		operator addrinfo**() {
			this->doWeClearAddrInfo = true;
			if (this->addrinfoPtrTwo == nullptr) {
				throw ConnectionError{ "addrinfoWrapper::addrinfo**(), addrinfoPtrTwo was nullptr." };
			}
			return &this->addrinfoPtrTwo;
		}

		operator addrinfo*() {
			if (this->addrinfoPtrTwo == nullptr) {
				throw ConnectionError{ "addrinfoWrapper::addrinfo*(), addrinfoPtrTwo was nullptr." };
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
			auto errorValue = SSL_CTX_up_ref(other);
			if (!errorValue) {
				std::cout << DiscordCoreAPI::shiftToBrightRed() << "SSL_CTX_up_ref() Error: " << ERR_error_string(errorValue, nullptr) << DiscordCoreAPI::reset() << std::endl;
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
			auto errorValue = SSL_up_ref(other);
			if (!errorValue) {
				std::cout << DiscordCoreAPI::shiftToBrightRed() << "SSL_up_ref() Error: " << ERR_error_string(errorValue, nullptr) << DiscordCoreAPI::reset() << std::endl;
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

	struct DiscordCoreAPI_Dll SOCKETWrapper {
		struct DiscordCoreAPI_Dll SOCKETDeleter {
			void operator()(SOCKET* other) {
				if (other) {
#ifdef _WIN32
					shutdown(*other, SD_BOTH);
					closesocket(*other);
#else
					shutdown(*other, SHUT_RDWR);
					close(*other);
#endif
					*other = SOCKET_ERROR;
				}
			}
		};

		SOCKETWrapper& operator=(SOCKETWrapper&& other) noexcept {
			if (this != &other) {
				this->socketPtr.swap(other.socketPtr);
				*other.socketPtr = SOCKET_ERROR;
			}
			return *this;
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
		std::unique_ptr<int32_t, SOCKETDeleter> socketPtr{ new SOCKET{ static_cast<int32_t>(SOCKET_ERROR) }, SOCKETDeleter{} };
	};

	class DiscordCoreAPI_Dll SSLEntity {
	  public:
		friend class DiscordCoreAPI::VoiceConnection;
		friend class WebSocketSSLShard;
		friend class VoiceSocketAgent;
		friend class BaseSocketAgent;

		virtual void writeData(const std::string& data, bool priority = false) = 0;

		virtual void connect(const std::string& baseUrl) = 0;

		virtual std::string getInputBuffer() noexcept = 0;

		virtual bool areWeStillConnected() noexcept = 0;

		virtual int64_t getBytesRead() noexcept = 0;

		virtual void reconnect() noexcept = 0;

		virtual ~SSLEntity() noexcept = default;

	  protected:
		std::unordered_map<uint64_t, DiscordCoreAPI::TSUnboundedMessageBlock<VoiceConnectionData>*> voiceConnectionDataBufferMap{};
		DiscordCoreAPI::StopWatch<std::chrono::milliseconds> heartBeatStopWatch{ 0ms };
		std::queue<DiscordCoreAPI::ConnectionPackage>* connections{ nullptr };
		WebSocketState theState{ WebSocketState::Connecting01 };
		std::queue<std::string> processedMessages{};
		std::atomic_bool areWeConnected02{ false };
		std::atomic_bool areWeConnected01{ false };
		VoiceConnectionData voiceConnectionData{};
		int32_t maxBufferSize{ (1024 * 16) - 1 };
		std::vector<std::string> outputBuffers{};
		bool haveWeReceivedHeartbeatAck{ true };
		bool serverUpdateCollected{ false };
		int32_t currentBaseSocketAgent{ 0 };
		bool stateUpdateCollected{ false };
		int32_t currentRecursionDepth{ 0 };
		SOCKETWrapper theSocket{ nullptr };
		bool areWeCollectingData{ false };
		int32_t maxRecursionDepth{ 10 };
		bool areWeHeartBeating{ false };
		int32_t lastNumberReceived{ 0 };
		WebSocketCloseCode closeCode{};
		WebSocketOpCode dataOpCode{};
		bool areWeResuming{ false };
		std::mutex accessorMutex{};
		SSLWrapper ssl{ nullptr };
		std::string inputBuffer{};
		int64_t messageLength{};
		int64_t messageOffset{};
		std::string sessionId{};
		nlohmann::json shard{};
		bool wantWrite{ true };
		bool wantRead{ false };
		int64_t bytesRead{ 0 };
		uint64_t userId{ 0 };
	};

	class DiscordCoreAPI_Dll HttpsSSLClient : public SSLEntity {
	  public:
		HttpsSSLClient() noexcept = default;

		static void initialize();

		void writeData(const std::string& data, bool priority = false) noexcept;

		void processIO(int32_t waitTimeInMs = 10000);

		void connect(const std::string& baseUrl);

		std::string getInputBuffer() noexcept;

		bool areWeStillConnected() noexcept;

		int64_t getBytesRead() noexcept;

		void reconnect() noexcept;

		~HttpsSSLClient() noexcept = default;

	  protected:

		static SSL_CTXWrapper context;

		int64_t connectionTime{ 0 };
	};

	class DiscordCoreAPI_Dll WebSocketSSLShard : public SSLEntity {
	  public:

		WebSocketSSLShard(std::queue<DiscordCoreAPI::ConnectionPackage>* connectionsNew, int32_t currentBaseSocketAgentNew, int32_t currentShardNew,
			DiscordCoreAPI::ConfigManager* configManagerNew) noexcept;

		static void processIO(std::unordered_map<int32_t, std::unique_ptr<SSLEntity>>& theMap, int32_t waitTimeInms = 10000);

		static void initialize();
		
		void writeData(const std::string& data, bool priority = false) noexcept;

		void connect(const std::string& baseUrl);

		std::string getInputBuffer() noexcept;

		bool areWeStillConnected() noexcept;

		int64_t getBytesRead() noexcept;

		void reconnect() noexcept;

		~WebSocketSSLShard() noexcept = default;

	  protected:
		static SSL_CTXWrapper context;
	};

	class DiscordCoreAPI_Dll DatagramSocketSSLClient {
	  public:
		DatagramSocketSSLClient() noexcept = default;

		void connect(const std::string& baseUrl, const std::string& portNew = "443");

		void writeData(const std::string& data, bool priority = false);

		std::string getInputBuffer() noexcept;

		int64_t getBytesRead() noexcept;

		void processIO();

		~DatagramSocketSSLClient() noexcept = default;

	  protected:
		const int32_t maxBufferSize{ 1024 * 16 };
		SOCKETWrapper theSocket{ nullptr };
		std::string outputBuffer{};
		std::string inputBuffer{};
		int64_t bytesRead{ 0 };
	};
}// namespace DiscordCoreInterna