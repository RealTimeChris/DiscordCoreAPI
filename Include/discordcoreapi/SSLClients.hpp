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

#pragma once

#ifndef SSL_CLIENTS
	#define SSL_CLIENTS

	#include <discordcoreapi/FoundationEntities.hpp>
	#include <discordcoreapi/EventEntities.hpp>

	#ifndef OPENSSL_NO_DEPRECATED
		#define OPENSSL_NO_DEPRECATED
	#endif

	#include <openssl/err.h>
	#include <openssl/ssl.h>

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
		#define SOCKET_ERROR (-1)
	#endif
	#ifdef _WIN32
	using SOCKET = Uint32;
	#else
	using SOCKET = Int32;
	#endif

	struct DiscordCoreAPI_Dll PollFDWrapper {
		UMap<Uint32, pollfd> thePolls{};
	};

	#ifdef _WIN32
	struct DiscordCoreAPI_Dll WSADataWrapper {
		struct DiscordCoreAPI_Dll WSADataDeleter {
			Void operator()(WSADATA* other);
		};

		WSADataWrapper();

	  protected:
		std::unique_ptr<WSADATA, WSADataDeleter> thePtr{ new WSADATA{}, WSADataDeleter{} };
	};
	#endif

	struct DiscordCoreAPI_Dll SSL_CTXWrapper {
		struct DiscordCoreAPI_Dll SSL_CTXDeleter {
			Void operator()(SSL_CTX* other);
		};

		SSL_CTXWrapper& operator=(SSL_CTX* other);

		operator SSL_CTX*();

		SSL_CTXWrapper() noexcept = default;

	  protected:
		std::unique_ptr<SSL_CTX, SSL_CTXDeleter> thePtr{ nullptr, SSL_CTXDeleter{} };
	};

	struct DiscordCoreAPI_Dll SSLWrapper {
		struct DiscordCoreAPI_Dll SSLDeleter {
			Void operator()(SSL* other);
		};

		SSLWrapper& operator=(SSL* other);

		operator SSL*();

		SSLWrapper() noexcept = default;

	  protected:
		std::unique_ptr<SSL, SSLDeleter> thePtr{ nullptr, SSLDeleter{} };
	};

	struct DiscordCoreAPI_Dll SOCKETWrapper {
		struct DiscordCoreAPI_Dll SOCKETDeleter {
			Void operator()(SOCKET* other);
		};

		SOCKETWrapper& operator=(SOCKETWrapper&&) noexcept;

		SOCKETWrapper(SOCKETWrapper&&) noexcept;

		SOCKETWrapper& operator=(SOCKET other) noexcept;

		SOCKETWrapper(SOCKET other) noexcept;

		operator SOCKET*() noexcept;

		operator SOCKET() noexcept;

		SOCKETWrapper() noexcept = default;

	  protected:
		std::unique_ptr<SOCKET, SOCKETDeleter> thePtr{ new SOCKET{ static_cast<SOCKET>(SOCKET_ERROR) }, SOCKETDeleter{} };
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
		Bool doWeClearAddrInfo{ false };
	};

	class DiscordCoreAPI_Dll SSLConnectionInterface {
	  public:
		virtual Bool connect(const String& baseUrl, const String& portNew, Bool doWePrintErrorMessages, Bool areWeAStandaloneSocket) noexcept = 0;

		virtual Bool areWeStillConnected() noexcept = 0;

		virtual Void disconnect(bool) noexcept = 0;

		static Bool initialize() noexcept;

		virtual ~SSLConnectionInterface() noexcept = default;

	  protected:
		static SSL_CTXWrapper context;
		static std::mutex theMutex;

		SOCKETWrapper theSocket{};
		SSLWrapper ssl{};
	};

	enum class ProcessIOResult : Int8 { No_Error = 0, Error = 1 };

	class DiscordCoreAPI_Dll SSLDataInterface {
	  public:
		friend class HttpsClient;

		virtual ProcessIOResult writeData(String& dataToWrite, Bool priority) noexcept = 0;

		virtual StringView getInputBuffer() noexcept = 0;

		virtual Int64 getBytesRead() noexcept = 0;

		virtual ~SSLDataInterface() noexcept = default;

	  protected:
		const Uint64 maxBufferSize{ (1024 * 16) - 1 };
		RingBuffer<32> outputBuffer{};
		RingBuffer<128> inputBuffer{};
		Int64 bytesRead{ 0 };
	};

	class DiscordCoreAPI_Dll SSLClient : public SSLDataInterface, public SSLConnectionInterface {
	  public:
		static Vector<SSLClient*> processIO(UMap<Uint32, std::unique_ptr<WebSocketSSLShard>>& theShardMap) noexcept;

		Bool connect(const String& baseUrl, const String& portNew, Bool doWePrintErrorMessages, Bool areWeAStandaloneSocket) noexcept;

		ProcessIOResult writeData(String& dataToWrite, Bool priority) noexcept;

		ProcessIOResult processIO(Int32 msToWait) noexcept;

		StringView getInputBuffer() noexcept;

		virtual Void handleBuffer() noexcept = 0;

		Bool areWeStillConnected() noexcept;

		Bool processWriteData() noexcept;

		Bool processReadData() noexcept;

		Int64 getBytesRead() noexcept;

		virtual ~SSLClient() noexcept = default;

	  protected:
		Bool doWePrintErrorMessages{ false };
		Bool areWeAStandaloneSocket{ false };
	};

	enum class ProcessIOType { Both = 0, Read_Only = 1, Write_Only = 2 };

	class DiscordCoreAPI_Dll DatagramSocketClient {
	  public:
		friend class DiscordCoreAPI::VoiceConnection;

		DatagramSocketClient(DiscordCoreAPI::StreamType streamType) noexcept;

		Bool connect(const String& baseUrlNew, const String& portNew) noexcept;

		ProcessIOResult processIO(ProcessIOType theType) noexcept;

		StringView getInputBuffer() noexcept;

		Void writeData(String data) noexcept;

		Bool areWeStillConnected() noexcept;

		Bool processWriteData() noexcept;

		Bool processReadData() noexcept;

		Int64 getBytesRead() noexcept;

		Void disconnect() noexcept;

	  protected:
		const Uint64 maxBufferSize{ (1024 * 16) - 1 };
		DiscordCoreAPI::StreamType streamType{};
		sockaddr_in theStreamTargetAddress{};
		Bool areWeStreamConnected{ false };
		RingBuffer<4> outputBuffer{};
		RingBuffer<4> inputBuffer{};
		SOCKETWrapper theSocket{};
		Int64 bytesRead{};
	};
}// namespace DiscordCoreInternal
#endif