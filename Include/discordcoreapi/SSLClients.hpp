/*
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

#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/EventEntities.hpp>
#include <openssl/err.h>
#include <openssl/ssl.h>

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

	#ifdef max
		#undef max
	#endif

	#ifdef min
		#undef min
	#endif
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

namespace DiscordCoreInternal {

	using SOCKET = int32_t;

#ifndef SOCKET_ERROR
	#define SOCKET_ERROR (-1)
#endif

	struct ConnectionError : public std::runtime_error {
		ConnectionError(const std::string& theString);
	};

#ifdef _WIN32
	struct DiscordCoreAPI_Dll WSADataWrapper {
		struct DiscordCoreAPI_Dll WSADataDeleter {
			void operator()(WSADATA* other);
		};

		WSADataWrapper();

	  protected:
		std::unique_ptr<WSADATA, WSADataDeleter> thePtr{ new WSADATA{}, WSADataDeleter{} };
	};
#endif

	struct DiscordCoreAPI_Dll SSL_CTXWrapper {
		struct DiscordCoreAPI_Dll SSL_CTXDeleter {
			void operator()(SSL_CTX* other);
		};

		SSL_CTXWrapper& operator=(SSL_CTX* other);

		operator SSL_CTX*();

		SSL_CTXWrapper() = default;

	  protected:
		std::unique_ptr<SSL_CTX, SSL_CTXDeleter> thePtr{ nullptr, SSL_CTXDeleter{} };
	};

	struct DiscordCoreAPI_Dll SSLWrapper {
		struct DiscordCoreAPI_Dll SSLDeleter {
			void operator()(SSL* other);
		};

		SSLWrapper& operator=(SSL* other);

		operator SSL*();

		SSLWrapper() = default;

	  protected:
		std::unique_ptr<SSL, SSLDeleter> thePtr{ nullptr, SSLDeleter{} };
	};

	struct DiscordCoreAPI_Dll SOCKETWrapper {
		struct DiscordCoreAPI_Dll SOCKETDeleter {
			void operator()(SOCKET* other);
		};

		SOCKETWrapper& operator=(SOCKETWrapper&& other) noexcept;

		SOCKETWrapper(SOCKETWrapper&& other) noexcept;

		SOCKETWrapper& operator=(SOCKET other);

		operator SOCKET();

		SOCKETWrapper() = default;

	  protected:
		std::unique_ptr<SOCKET, SOCKETDeleter> thePtr{ new SOCKET{ SOCKET_ERROR }, SOCKETDeleter{} };
	};

	struct DiscordCoreAPI_Dll addrinfoWrapper {
		addrinfo* operator->();

		operator addrinfo**();

		operator addrinfo*();

		addrinfoWrapper() = default;

		~addrinfoWrapper();

	  protected:
		addrinfo* thePtr{ new addrinfo{} };
		bool doWeClearAddrInfo{ false };
	};

	enum class SSLConnectionState { Connected = 1, Disconnected = 2 };

	class DiscordCoreAPI_Dll SSLConnectionInterface {
	  public:
		SSLConnectionInterface() noexcept = default;

		static void initialize();

		virtual bool connect(const std::string& baseUrl, const std::string& portNew) = 0;

		virtual void disconnect(bool doWeReconnect) noexcept = 0;

		virtual bool areWeStillConnected() noexcept = 0;

		virtual ~SSLConnectionInterface() noexcept = default;

	  protected:
		static SSL_CTXWrapper context;
		static std::mutex theMutex;

		std::atomic<SSLConnectionState> theSSLState{ SSLConnectionState::Disconnected };
		std::queue<DiscordCoreAPI::ConnectionPackage>* connections{ nullptr };
		std::recursive_mutex rwMutex{};
		SOCKETWrapper theSocket{};
		SSLWrapper ssl{};
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
		std::mutex writeMutex{};
		bool wantWrite{ true };
		bool wantRead{ false };
		int64_t bytesRead{ 0 };
		std::mutex readMutex{};
	};

	class DiscordCoreAPI_Dll SSLEntity : public SSLDataInterface, public SSLConnectionInterface {
	  public:
		SSLEntity() noexcept = default;

		static void processIO(std::unordered_map<int32_t, std::unique_ptr<SSLEntity>>& theMap, int32_t waitTimeInms = 1000) noexcept;

		void processIO(int32_t waitTimeInMs = 10000) noexcept;

	  protected:
	};

	class DiscordCoreAPI_Dll HttpsSSLClient : public SSLEntity {
	  public:
		HttpsSSLClient() noexcept = default;

		[[nodiscard]] bool connect(const std::string& baseUrl, const std::string& portNew) noexcept;

		bool writeData(const std::string& data, bool priority = false) noexcept;

		void disconnect(bool doWeReconnect) noexcept;

		std::string getInputBuffer() noexcept;

		bool areWeStillConnected() noexcept;

		int64_t getBytesRead() noexcept;

		~HttpsSSLClient() noexcept = default;
	};

	enum class WebSocketSSLShardState { Connecting = 0, Upgrading = 1, Collecting_Hello = 2, Sending_Identify = 3, Authenticated = 4, Disconnected = 5 };

	class DiscordCoreAPI_Dll WebSocketSSLShard : public SSLEntity {
	  public:
		friend class DiscordCoreAPI::VoiceConnection;
		friend class WebSocketMessageHandler;
		friend class DiscordCoreAPI::BotUser;
		friend class BaseSocketAgent;
		friend class YouTubeAPI;

		WebSocketSSLShard(std::queue<DiscordCoreAPI::ConnectionPackage>* connectionsNew, int32_t currentBaseSocketAgentNew, int32_t currentShardNew,
			DiscordCoreAPI::ConfigManager* configManagerNew) noexcept;

		bool connect(const std::string& baseUrl, const std::string& portNew) noexcept;

		bool writeData(const std::string& data, bool priority = false) noexcept;

		void disconnect(bool doWeReconnect) noexcept;

		std::string getInputBuffer() noexcept;

		bool areWeStillConnected() noexcept;

		int64_t getBytesRead() noexcept;

		~WebSocketSSLShard() noexcept = default;

	  protected:
		std::unordered_map<Snowflake, DiscordCoreAPI::UnboundedMessageBlock<VoiceConnectionData>*> voiceConnectionDataBufferMap{};
		std::atomic<WebSocketSSLShardState> theWebSocketState{ WebSocketSSLShardState ::Connecting };
		DiscordCoreAPI::StopWatch<std::chrono::milliseconds> heartBeatStopWatch{ 0ms };
		std::queue<std::string> processedMessages{};
		VoiceConnectionData voiceConnectionData{};
		bool haveWeReceivedHeartbeatAck{ true };
		bool serverUpdateCollected{ false };
		int32_t currentBaseSocketAgent{ 0 };
		int32_t currentReconnectTries{ 0 };
		bool stateUpdateCollected{ false };
		bool areWeCollectingData{ false };
		std::recursive_mutex theMutex{};
		bool areWeHeartBeating{ false };
		int32_t lastNumberReceived{ 0 };
		WebSocketCloseCode closeCode{};
		WebSocketOpCode dataOpCode{};
		bool areWeResuming{ false };
		int64_t messageLength{};
		int64_t messageOffset{};
		std::string sessionId{};
		nlohmann::json shard{};
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

		void readData() noexcept;

		~DatagramSocketSSLClient() noexcept = default;

	  protected:
		const int32_t maxBufferSize{ 1024 * 16 };
		std::vector<std::string> outputBuffers{};
		std::recursive_mutex theMutex{};
		SOCKETWrapper theSocket{};
		std::string inputBuffer{};
		sockaddr_in theAddress{};
		int64_t bytesRead{};
	};
}// namespace DiscordCoreInterna