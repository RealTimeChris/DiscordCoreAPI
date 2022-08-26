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

#ifdef SOCKET_ERROR
	#undef SOCKET_ERROR
#endif

	constexpr int32_t SOCKET_ERROR{ -1 };

	using SOCKET = int32_t;

	struct ConnectionError : public std::runtime_error {
		int32_t shardNumber{};
		explicit ConnectionError(const std::string& theString);
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

		SSL_CTXWrapper() noexcept = default;

	  protected:
		std::unique_ptr<SSL_CTX, SSL_CTXDeleter> thePtr{ nullptr, SSL_CTXDeleter{} };
	};

	struct DiscordCoreAPI_Dll SSLWrapper {
		struct DiscordCoreAPI_Dll SSLDeleter {
			void operator()(SSL* other);
		};

		SSLWrapper& operator=(SSL* other);

		operator SSL*();

		SSLWrapper() noexcept = default;

	  protected:
		std::unique_ptr<SSL, SSLDeleter> thePtr{ nullptr, SSLDeleter{} };
	};

	struct DiscordCoreAPI_Dll SOCKETWrapper {
		struct DiscordCoreAPI_Dll SOCKETDeleter {
			void operator()(SOCKET* other);
		};

		SOCKETWrapper& operator=(SOCKET other);

		operator SOCKET*();

		operator SOCKET();

		SOCKETWrapper() noexcept = default;

	  protected:
		std::unique_ptr<SOCKET, SOCKETDeleter> thePtr{ nullptr, SOCKETDeleter{} };
	};

	struct DiscordCoreAPI_Dll sockaddrWrapper {
		sockaddr* operator->();

		operator sockaddr_in*();

		operator sockaddr*();

		sockaddrWrapper() noexcept = default;

		~sockaddrWrapper() noexcept = default;

	  protected:
		sockaddr_in thePtr{};
	};

	struct DiscordCoreAPI_Dll addrinfoWrapper {
		addrinfo* operator->();

		operator addrinfo**();

		operator addrinfo*();

		addrinfoWrapper() noexcept = default;

		~addrinfoWrapper();

	  protected:
		addrinfo* thePtr{ new addrinfo{} };
		bool doWeClearAddrInfo{ false };
	};

	class DiscordCoreAPI_Dll SSLConnectionInterface {
	  public:
		SSLConnectionInterface() noexcept = default;

		virtual bool connect(const std::string& baseUrl, const std::string& portNew, bool doWePrintErrorMessages) = 0;

		virtual void disconnect(bool doWeReconnect) noexcept = 0;

		virtual bool areWeStillConnected() noexcept = 0;

		virtual ~SSLConnectionInterface() noexcept;

	  protected:
		std::recursive_mutex connectionMutex{};
		SOCKETWrapper theSocket{};
		SSL_CTXWrapper context{};
		SSLWrapper ssl{};
	};

	enum class ProcessIOResult : int8_t { No_Error = 0, Disconnected = 1, SSL_Error = 2, SSL_Zero_Return = 3, Select_Failure = 4, No_Return = 5, Nothing_To_Write = 6 };

	class DiscordCoreAPI_Dll SSLDataInterface {
	  public:
		friend class HttpsClient;

		SSLDataInterface() noexcept = default;

		virtual ProcessIOResult writeData(std::string& dataToWrite, bool priority) noexcept = 0;

		virtual std::string& getInputBuffer() noexcept = 0;

		virtual int64_t getBytesRead() noexcept = 0;

		virtual ~SSLDataInterface() noexcept = default;

	  protected:
		int32_t maxBufferSize{ (1024 * 16) - 1 };
		std::deque<std::string> outputBuffers{};
		std::string rawInputBuffer{};
		std::string inputBuffer{};
		bool wantWrite{ true };
		bool wantRead{ false };
		int64_t bytesRead{ 0 };
	};

	class DiscordCoreAPI_Dll SSLClient : public SSLDataInterface, public SSLConnectionInterface {
	  public:
		SSLClient() noexcept = default;

		bool connect(const std::string& baseUrl, const std::string& portNew, bool doWePrintErrorMessages);

		ProcessIOResult writeData(std::string& dataToWrite, bool priority) noexcept;

		static std::vector<ConnectionError> processIO(std::vector<SSLClient*>&);

		ProcessIOResult processIO(int32_t msToWait);

		std::string& getInputBuffer() noexcept;

		bool areWeStillConnected() noexcept;

		ProcessIOResult writeDataProcess();

		ProcessIOResult readDataProcess();

		int64_t getBytesRead() noexcept;

		virtual ~SSLClient() noexcept = default;

	  protected:
		bool doWePrintErrorMessages{ false };
	};

	enum class ProcessIOType { Both = 0, Read_Only = 1, Write_Only = 2 };

	class DiscordCoreAPI_Dll DatagramSocketClient {
	  public:
		friend class DiscordCoreAPI::VoiceConnection;

		DatagramSocketClient(DiscordCoreAPI::StreamType streamType) noexcept;

		bool connect(const std::string& baseUrlNew, const std::string& portNew) noexcept;

		void processIO(ProcessIOType theType) noexcept;

		void writeData(std::string data) noexcept;

		std::string& getInputBuffer() noexcept;

		bool areWeStillConnected() noexcept;

		void writeDataProcess() noexcept;

		void readDataProcess() noexcept;

		int64_t getBytesRead() noexcept;

		void disconnect() noexcept;

		virtual ~DatagramSocketClient() noexcept;

	  protected:
		const int32_t maxBufferSize{ (1024 * 16) - 1 };
		sockaddrWrapper theStreamCurrentAddress{};
		sockaddrWrapper theStreamTargetAddress{};
		std::deque<std::string> outputBuffers{};
		DiscordCoreAPI::StreamType streamType{};
		bool areWeStreamConnected{ false };
		std::recursive_mutex theMutex{};
		std::string rawInputBuffer{};
		SOCKETWrapper theSocket{};
		std::string inputBuffer{};
		int64_t bytesRead{};
	};
}// namespace DiscordCoreInternal