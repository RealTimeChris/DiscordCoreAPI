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

#ifdef SOCKET_ERROR
	#undef SOCKET_ERROR
#endif

	using SOCKET = uint32_t;

	constexpr int32_t SOCKET_ERROR{ -1 };

	struct PollFDWrapper {
		std::vector<uint32_t> theIndices{};
		std::vector<pollfd> thePolls{};
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
			void operator()(SOCKET other);
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

		~SOCKETWrapper() noexcept;

	  protected:
		SOCKET thePtr{ static_cast<SOCKET>(SOCKET_ERROR) };
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

	enum class ConnectionResult : int8_t { No_Error = 0, Error = 1 };

	class DiscordCoreAPI_Dll SSLConnectionInterface {
	  public:
		SSLConnectionInterface() noexcept = default;

		virtual ConnectionResult connect(const std::string& baseUrl, const std::string& portNew, bool doWePrintErrorMessages, bool areWeAStandaloneSocket) noexcept = 0;

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

	struct DiscordCoreAPI_Dll RingBuffer {
		std::array<char, 1024 * 1024 * 2> theArray{};
		int64_t head{};
		int64_t tail{};
		char& peekByte(size_t theIndex);
		void putByte(char& theByte);
		char getByte();
		char* getCurrentTail();
		char* getCurrentHead();
		int64_t getFreeSpace();
		int64_t getUsedSpace();
		bool isItEmpty();
		bool isItFull();
		void clear();
	};

	class DiscordCoreAPI_Dll SSLDataInterface {
	  public:
		friend class HttpsClient;

		SSLDataInterface() noexcept;

		virtual ProcessIOResult writeData(std::string& dataToWrite, bool priority) noexcept = 0;

		virtual std::string_view getInputBuffer(uint32_t offSet, uint32_t length) noexcept = 0;

		virtual std::string_view getInputBuffer() noexcept = 0;

		virtual int64_t getBytesRead() noexcept = 0;

		virtual ~SSLDataInterface() noexcept = default;

	  protected:
		std::array<char, 1024 * 16> rawInputBuffer{};
		int32_t maxBufferSize{ (1024 * 16) - 1 };
		std::deque<std::string> outputBuffers{};
		std::string currentMessageBuffer{};
		uint64_t offsetIntoInputBuffer{};
		RingBuffer inputBuffer{};
		int64_t bytesRead{ 0 };
	};

	enum class SSLShardState { Connecting = 0, Upgrading = 1, Collecting_Hello = 2, Sending_Identify = 3, Authenticated = 4, Disconnected = 5 };

	class DiscordCoreAPI_Dll SSLClient : public SSLDataInterface, public SSLConnectionInterface {
	  public:
		SSLClient() noexcept = default;

		ConnectionResult connect(const std::string& baseUrl, const std::string& portNew, bool doWePrintErrorMessages, bool areWeAStandaloneSocket) noexcept;

		static std::vector<SSLClient*> processIO(std::vector<SSLClient*>&) noexcept;

		ProcessIOResult writeData(std::string& dataToWrite, bool priority) noexcept;

		std::string_view getInputBuffer(uint32_t offSet, uint32_t length) noexcept;

		ProcessIOResult processIO(int32_t msToWait) noexcept;
		
		virtual bool handleBuffer(SSLClient*) noexcept = 0;

		std::string_view getInputBufferRemove() noexcept;

		std::string_view getInputBuffer() noexcept;

		bool areWeStillConnected() noexcept;

		bool processWriteData() noexcept;

		bool processReadData() noexcept;

		int64_t getBytesRead() noexcept;

		virtual ~SSLClient() noexcept = default;

	  protected:
		std::atomic<SSLShardState> currentState{};
		bool doWePrintErrorMessages{ false };
		bool areWeAStandaloneSocket{ false };
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

		void processWriteData() noexcept;

		void processReadData() noexcept;

		int64_t getBytesRead() noexcept;

		void disconnect() noexcept;

		virtual ~DatagramSocketClient() noexcept;

	  protected:
		const int32_t maxBufferSize{ (1024 * 16) - 1 };
		std::array<char, 1024 * 16> rawInputBuffer{};
		std::deque<std::string> outputBuffers{};
		DiscordCoreAPI::StreamType streamType{};
		sockaddr_in theStreamTargetAddress{};
		bool areWeStreamConnected{ false };
		SOCKETWrapper theSocket{};
		std::string inputBuffer{};
		int64_t bytesRead{};
	};
}// namespace DiscordCoreInternal
#endif