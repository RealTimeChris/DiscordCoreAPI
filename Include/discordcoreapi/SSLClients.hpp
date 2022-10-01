/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022 Chris M. (RealTimeChris)

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
	USA
*/
/// SSLClents.hpp - Header file for the "SSL Client" stuff.
/// Dec 12, 2021
/// https://discordcoreapi.com
/// \file SSLClients.hpp

#ifndef SSL_CLIENTS
#define SSL_CLIENTS

#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/EventEntities.hpp>
#include <openssl/err.h>
#include <openssl/ssl.h>

#ifndef OPENSSL_NO_DEPRECATED
	#define OPENSSL_NO_DEPRECATED
#endif

#ifdef _WIN32
	#pragma comment(lib, "Ws2_32.lib")
	#include <WinSock2.h>
	#include <WS2tcpip.h>
	#define poll(fd_set, fd_count, timeout) WSAPoll(fd_set, fd_count, timeout)
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
	#include <poll.h>
	#include <arpa/inet.h>
	#include <sys/socket.h>
	#include <sys/types.h>
	#include <unistd.h>
#endif

namespace DiscordCoreInternal {

#ifndef SOCKET_ERROR
	#define SOCKET_ERROR - 1
#endif

	using INT_PTR = signed long long;

	using SOCKET = INT_PTR;

	struct DiscordCoreAPI_Dll PollFDWrapper {
		std::unordered_map<uint32_t, pollfd> thePolls{};
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

		SOCKETWrapper& operator=(SOCKETWrapper&&) noexcept;

		SOCKETWrapper(SOCKETWrapper&&) noexcept;

		SOCKETWrapper& operator=(const SOCKETWrapper&) noexcept = delete;

		SOCKETWrapper(const SOCKETWrapper&) noexcept = delete;

		SOCKETWrapper& operator=(SOCKET other) noexcept;

		SOCKETWrapper(SOCKET other) noexcept;

		operator SOCKET*() noexcept;

		operator SOCKET() noexcept;

		SOCKETWrapper() noexcept = default;

	  protected:
		std::unique_ptr<SOCKET, SOCKETDeleter> thePtr{ new SOCKET{ SOCKET_ERROR }, SOCKETDeleter{} };
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

		virtual bool connect(const std::string& baseUrl, const std::string& portNew, bool doWePrintErrorMessages, bool areWeAStandaloneSocket) noexcept = 0;

		virtual void disconnect(bool doWeReconnect) noexcept = 0;

		virtual bool areWeStillConnected() noexcept = 0;

		static bool initialize() noexcept;

		virtual ~SSLConnectionInterface() noexcept = default;

	  protected:
		static SSL_CTXWrapper context;
		static std::mutex theMutex;

		SOCKETWrapper theSocket{};
		SSLWrapper ssl{};
	};

	enum class ProcessIOResult : int8_t { No_Error = 0, Error = 1 };

	class DiscordCoreAPI_Dll SSLDataInterface {
	  public:
		friend class HttpsClient;

		virtual ProcessIOResult writeData(std::string& dataToWrite, bool priority) noexcept = 0;

		virtual std::string_view getInputBuffer() noexcept = 0;

		virtual int64_t getBytesRead() noexcept = 0;

		virtual ~SSLDataInterface() noexcept = default;

	  protected:
		RingBuffer outputBuffer{};
		RingBuffer inputBuffer{};
		int64_t bytesRead{ 0 };
	};

	class DiscordCoreAPI_Dll SSLClient : public SSLDataInterface, public SSLConnectionInterface {
	  public:
		SSLClient() noexcept;

		bool connect(const std::string& baseUrl, const std::string& portNew, bool doWePrintErrorMessages, bool areWeAStandaloneSocket) noexcept;

		static std::vector<SSLClient*> processIO(std::unordered_map<uint32_t, std::unique_ptr<WebSocketSSLShard>>& theShardMap) noexcept;

		ProcessIOResult writeData(std::string& dataToWrite, bool priority) noexcept;

		ProcessIOResult processIO(int32_t msToWait) noexcept;

		std::string_view getInputBuffer() noexcept;

		virtual void handleBuffer() noexcept = 0;

		bool areWeStillConnected() noexcept;

		bool processWriteData() noexcept;

		bool processReadData() noexcept;

		int64_t getBytesRead() noexcept;

		virtual ~SSLClient() noexcept = default;

	  protected:
		bool doWePrintErrorMessages{ false };
		bool areWeAStandaloneSocket{ false };
	};

	enum class ProcessIOType { Both = 0, Read_Only = 1, Write_Only = 2 };

	class DiscordCoreAPI_Dll DatagramSocketClient {
	  public:
		friend class DiscordCoreAPI::VoiceConnection;

		DatagramSocketClient(DiscordCoreAPI::StreamType streamType) noexcept;

		bool connect(const std::string& baseUrlNew, const std::string& portNew) noexcept;

		ProcessIOResult processIO(ProcessIOType theType) noexcept;

		std::string_view getInputBuffer() noexcept;

		void writeData(std::string data) noexcept;

		bool areWeStillConnected() noexcept;

		bool processWriteData() noexcept;

		bool processReadData() noexcept;

		int64_t getBytesRead() noexcept;

		void disconnect() noexcept;

	  protected:
		const int32_t maxBufferSize{ (1024 * 16) - 1 };
		std::array<char, 1024 * 16> inputBuffer{};
		DiscordCoreAPI::StreamType streamType{};
		sockaddr_in theStreamTargetAddress{};
		bool areWeStreamConnected{ false };
		LightRingBuffer outputBuffer{};
		size_t currentlyUsedSpace{};
		SOCKETWrapper theSocket{};
		int64_t bytesRead{};
	};
}// namespace DiscordCoreInternal
#endif