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
	#ifdef errno
		#undef errno
		#define errno WSAGetLastError()
	#endif
	#ifdef EWOULDBLOCK
		#undef EWOULDBLOCK
		#define EWOULDBLOCK WSAEWOULDBLOCK
	#endif
	#define poll(fd_set, fd_count, timeout) WSAPoll(fd_set, fd_count, timeout)
	#define pollfd WSAPOLLFD
	#define close closesocket
	#define SHUT_RDWR SD_BOTH
	#ifdef max
		#undef max
	#endif
	#ifdef min
		#undef min
	#endif
#elif __linux__
	#include <netinet/tcp.h>
	#include <netinet/in.h>
	#include <sys/socket.h>
	#include <sys/types.h>
	#include <netdb.h>
	#include <fcntl.h>
	#include <poll.h>
#endif

namespace DiscordCoreInternal {

#ifndef SOCKET_ERROR
	#define SOCKET_ERROR (-1)
#endif

#ifdef _WIN32
	using SOCKET = uint32_t;
#else
	using SOCKET = int32_t;
#endif

	struct DiscordCoreAPI_Dll PollFDWrapper {
		std::vector<size_t> indices{};
		std::vector<pollfd> polls{};
	};

#ifdef _WIN32
	struct DiscordCoreAPI_Dll WSADataWrapper {
		struct DiscordCoreAPI_Dll WSADataDeleter {
			void operator()(WSADATA* other);
		};

		WSADataWrapper();

	  protected:
		std::unique_ptr<WSADATA, WSADataDeleter> ptr{ std::make_unique<WSADATA>().release(), WSADataDeleter{} };
	};
#endif

	struct DiscordCoreAPI_Dll SSL_CTXWrapper {
		struct DiscordCoreAPI_Dll SSL_CTXDeleter {
			void operator()(SSL_CTX* other);
		};

		SSL_CTXWrapper& operator=(SSL_CTX* other);

		operator SSL_CTX*();

	  protected:
		std::unique_ptr<SSL_CTX, SSL_CTXDeleter> ptr{ nullptr, SSL_CTXDeleter{} };
	};

	struct DiscordCoreAPI_Dll SSLWrapper {
		struct DiscordCoreAPI_Dll SSLDeleter {
			void operator()(SSL* other);
		};

		SSLWrapper& operator=(nullptr_t other);

		SSLWrapper& operator=(SSL* other);

		operator SSL*();

	  protected:
		std::unique_ptr<SSL, SSLDeleter> ptr{ nullptr, SSLDeleter{} };
	};

	struct DiscordCoreAPI_Dll SOCKETWrapper {
		struct DiscordCoreAPI_Dll SOCKETDeleter {
			void operator()(SOCKET* other);
		};

		SOCKETWrapper& operator=(SOCKETWrapper&&) noexcept;

		SOCKETWrapper(SOCKETWrapper&&) noexcept;

		SOCKETWrapper& operator=(SOCKET other) noexcept;

		SOCKETWrapper(SOCKET other) noexcept;

		operator SOCKET*() noexcept;

		operator SOCKET() noexcept;

		SOCKETWrapper() noexcept = default;

	  protected:
		std::unique_ptr<SOCKET, SOCKETDeleter> ptr{ std::make_unique<SOCKET>(SOCKET_ERROR).release(), SOCKETDeleter{} };
	};

	struct DiscordCoreAPI_Dll addrinfoWrapper {
		addrinfo* operator->();

		operator addrinfo**();

		operator addrinfo*();

	  protected:
		addrinfo value{};
		addrinfo* ptr{ &this->value };
	};

	class DiscordCoreAPI_Dll SSLConnectionInterface {
	  public:
		virtual bool connect(const std::string& baseUrl, const uint16_t portNew, bool doWePrintErrorMessages,
			bool areWeAStandaloneSocket) noexcept = 0;

		virtual bool areWeStillConnected() noexcept = 0;

		virtual void disconnect() noexcept = 0;

		static bool initialize() noexcept;

		virtual ~SSLConnectionInterface() noexcept = default;

	  protected:
		static SSL_CTXWrapper context;
		static std::mutex mutex;

		SOCKETWrapper socket{};
		SSLWrapper ssl{};
	};

	enum class ProcessIOResult : uint8_t { No_Error = 0, Error = 1 };

	class DiscordCoreAPI_Dll SSLDataInterface {
	  public:
		friend class HttpsClient;

		virtual ProcessIOResult writeData(std::string_view dataToWrite, bool priority) noexcept = 0;

		virtual std::string_view getInputBuffer() noexcept = 0;

		virtual ~SSLDataInterface() noexcept = default;

	  protected:
		const uint64_t maxBufferSize{ (1024 * 16) - 1 };
		RingBuffer<char, 16> outputBuffer{};
		RingBuffer<char, 64> inputBuffer{};
		int64_t bytesRead{};
	};

	class DiscordCoreAPI_Dll TCPSSLClient : public SSLDataInterface, public SSLConnectionInterface {
	  public:
		virtual void handleBuffer() noexcept = 0;

		bool connect(const std::string& baseUrl, const uint16_t portNew, bool doWePrintErrorMessages, bool areWeAStandaloneSocket) noexcept;

		static std::vector<TCPSSLClient*> processIO(std::unordered_map<uint32_t, std::unique_ptr<WebSocketClient>>& shardMap) noexcept;

		ProcessIOResult writeData(std::string_view dataToWrite, bool priority) noexcept;

		ProcessIOResult processIO(int32_t msToWait) noexcept;

		std::string_view getInputBuffer() noexcept;

		bool areWeStillConnected() noexcept;

		bool processWriteData() noexcept;

		bool processReadData() noexcept;

		int64_t getBytesRead() noexcept;

		virtual ~TCPSSLClient() noexcept = default;

	  protected:
		bool doWePrintErrorMessages{};
		bool areWeAStandaloneSocket{};
	};

	enum class ProcessIOType { Both = 0, Read_Only = 1, Write_Only = 2 };

	class DiscordCoreAPI_Dll UDPConnection {
	  public:
		friend class DiscordCoreAPI::VoiceConnection;

		UDPConnection(DiscordCoreAPI::StreamType streamType, bool doWePrintErrors) noexcept;

		bool connect(const std::string& baseUrlNew, uint16_t portNew, bool haveWeGottenSignaled, std::stop_token token = std::stop_token{}) noexcept;

		void writeData(std::basic_string_view<std::byte> dataToWrite) noexcept;

		std::basic_string_view<std::byte> getInputBuffer() noexcept;

		ProcessIOResult processIO(ProcessIOType type) noexcept;

		virtual void handleAudioBuffer() noexcept = 0;

		bool areWeStillConnected() noexcept;

		bool processWriteData() noexcept;

		bool processReadData() noexcept;

		void disconnect() noexcept;

		virtual ~UDPConnection() noexcept = default;

	  protected:
		const uint64_t maxBufferSize{ (1024 * 16) };
		RingBuffer<std::byte, 16> outputBuffer{};
		RingBuffer<std::byte, 16> inputBuffer{};
		DiscordCoreAPI::StreamType streamType{};
		addrinfoWrapper address{};
		bool doWePrintErrors{};
		SOCKETWrapper socket{};
		std::string baseUrl{};
		int64_t bytesRead{};
		uint16_t port{};
	};
}
