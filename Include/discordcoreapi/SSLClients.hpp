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
	#include <WinSock2.h>
	#include <WS2tcpip.h>

#else
	#include <fcntl.h>
	#include <netdb.h>
	#include <netinet/in.h>
	#include <netinet/tcp.h>
	#include <stdint.h>
	#include <sys/epoll.h>
	#include <arpa/inet.h>
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
				std::cout << DiscordCoreAPI::shiftToBrightRed() << "WSAStartup() Error: " << errorValue << ", ()";
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

	enum class SOCKETType { TCP = 0, UDP = 1 };

	struct SOCKETWrapperInternal {
		SOCKETType theType{ SOCKETType::TCP };
		SOCKET theSocket{ SOCKET_ERROR };
	};

	struct DiscordCoreAPI_Dll SOCKETWrapper {
		struct DiscordCoreAPI_Dll SOCKETDeleter {
			void operator()(SOCKET* other) {
#ifdef _WIN32
				shutdown(*other, SD_BOTH);
				closesocket(*other);
#else
				shutdown(*other, SHUT_RDWR);
				close(*other);
#endif

				*other = SOCKET_ERROR;
				delete other;
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
			this->socketPtr.reset(new SOCKET{ other });
			return *this;
		}

		operator SOCKET() {
			return *this->socketPtr;
		}

		SOCKETWrapper(std::nullptr_t) {
		}

	  protected:
		std::unique_ptr<SOCKET, SOCKETDeleter> socketPtr{ new SOCKET{ SOCKET_ERROR }, SOCKETDeleter{} };
	};

	class DiscordCoreAPI_Dll SSLConnectionInterface {
	  public:
		SSLConnectionInterface() noexcept = default;

		static void initialize();

		virtual bool connect(const std::string& baseUrl, const std::string& portNew) = 0;

		virtual bool areWeStillConnected() noexcept = 0;

		virtual void disconnect() noexcept = 0;

		virtual ~SSLConnectionInterface() noexcept = default;

	  protected:
		static SSL_CTXWrapper context;
		static std::mutex theMutex;

		std::queue<DiscordCoreAPI::ConnectionPackage>* connections{ nullptr };
		std::atomic_bool areWeConnected02{ false };
		std::atomic_bool areWeConnected01{ false };
		SOCKETWrapper theSocket{ nullptr };
		SSLWrapper ssl{ nullptr };
	};

	class DiscordCoreAPI_Dll SSLDataInterface {
	  public:
		SSLDataInterface() noexcept = default;

		virtual bool writeData(const std::string& data, bool priority = false) noexcept = 0;

		virtual std::string getInputBuffer() noexcept = 0;

		virtual int64_t getBytesRead() noexcept = 0;

		virtual ~SSLDataInterface() noexcept = default;

	  protected:
		int32_t maxBufferSize{ (1024 * 16) - 1 };
		std::vector<std::string> outputBuffers{};
		std::string inputBuffer{};
		bool wantWrite{ true };
		bool wantRead{ false };
		int64_t bytesRead{ 0 };
	};

	class DiscordCoreAPI_Dll HttpsSSLClient : public SSLConnectionInterface, public SSLDataInterface {
	  public:
		HttpsSSLClient() noexcept = default;

		[[nodiscard]] bool connect(const std::string& baseUrl, const std::string& portNew) noexcept;

		bool writeData(const std::string& data, bool priority = false) noexcept;

		void processIO(int32_t waitTimeInMs = 10000) noexcept;

		std::string getInputBuffer() noexcept;

		bool areWeStillConnected() noexcept;

		int64_t getBytesRead() noexcept;

		void disconnect() noexcept;

		~HttpsSSLClient() noexcept = default;
	};

	class DiscordCoreAPI_Dll WebSocketSSLShard : public SSLConnectionInterface, public SSLDataInterface {
	  public:
		friend class DiscordCoreAPI::VoiceConnection;
		friend class BaseSocketAgent;
		friend class YouTubeAPI;

		WebSocketSSLShard(std::queue<DiscordCoreAPI::ConnectionPackage>* connectionsNew, int32_t currentBaseSocketAgentNew, int32_t currentShardNew,
			DiscordCoreAPI::ConfigManager* configManagerNew, bool blocking = false) noexcept;

		static void processIO(std::unordered_map<int32_t, std::unique_ptr<WebSocketSSLShard>>& theMap, int32_t waitTimeInms = 1000) noexcept;

		bool connect(const std::string& baseUrl, const std::string& portNew) noexcept;

		bool writeData(const std::string& data, bool priority = false) noexcept;

		std::string getInputBuffer() noexcept;

		bool areWeStillConnected() noexcept;

		int64_t getBytesRead() noexcept;

		void disconnect() noexcept;

		~WebSocketSSLShard() noexcept = default;

	  protected:
		std::unordered_map<Snowflake, DiscordCoreAPI::UnboundedMessageBlock<VoiceConnectionData>*> voiceConnectionDataBufferMap{};
		DiscordCoreAPI::StopWatch<std::chrono::milliseconds> heartBeatStopWatch{ 0ms };
		WebSocketState theState{ WebSocketState::Connecting01 };
		std::queue<std::string> processedMessages{};
		VoiceConnectionData voiceConnectionData{};
		bool haveWeReceivedHeartbeatAck{ true };
		int32_t currentReconnectionTries{ 0 };
		bool serverUpdateCollected{ false };
		int32_t currentBaseSocketAgent{ 0 };
		bool stateUpdateCollected{ false };
		bool areWeCollectingData{ false };
		std::recursive_mutex theMutex02{};
		bool areWeHeartBeating{ false };
		int32_t lastNumberReceived{ 0 };
		WebSocketCloseCode closeCode{};
		WebSocketOpCode dataOpCode{};
		bool areWeResuming{ false };
		std::mutex theMutex01{};
		int64_t messageLength{};
		int64_t messageOffset{};
		std::string sessionId{};
		nlohmann::json shard{};
		bool blocking{ false };
		Snowflake userId{ 0 };
	};

	class DiscordCoreAPI_Dll DatagramSocketSSLClient {
	  public:
		friend class DiscordCoreAPI::VoiceConnection;

		DatagramSocketSSLClient() noexcept = default;

		bool connect(const std::string& baseUrl, const std::string& portNew) noexcept;

		void writeData(std::string& data) noexcept;

		std::string getInputBuffer() noexcept;

		bool areWeStillConnected() noexcept;

		int64_t getBytesRead() noexcept;

		void disconnect() noexcept;

		void processIO() noexcept;

		~DatagramSocketSSLClient() noexcept = default;

	  protected:
		std::atomic_bool areWeConnected{ false };
		const int32_t maxBufferSize{ 1024 * 16 };
		std::vector<std::string> outputBuffers{};
		SOCKETWrapper theSocket{ nullptr };
		std::string inputBuffer{};
		sockaddr_in theAddress{};
		int64_t bytesRead{};
	};
}// namespace DiscordCoreInterna