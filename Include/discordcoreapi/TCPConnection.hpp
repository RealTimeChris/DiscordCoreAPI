/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022, 2023 Chris M. (RealTimeChris)

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
/// TCPConnection.hpp - Header file for the "TCP Connection" stuff.
/// Dec 12, 2021
/// https://discordcoreapi.com
/// \file TCPConnection.hpp

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
	#define poll(fd_set, fd_count, timeout) WSAPoll(fd_set, fd_count, timeout)
	#define pollfd WSAPOLLFD
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

#ifndef INVALID_SOCKET
	#define INVALID_SOCKET SOCKET(~0)
#endif

#ifndef _WIN32
	using SOCKET = int32_t;
#endif

	enum class ConnectionErrorType { POLLHUP_Error = 0, POLLNVAL_Error = 1, POLLERR_Error = 2, READ_Error = 3, WRITE_Error = 4, SOCKET_Error = 5 };

	struct DiscordCoreAPI_Dll ConnectionError : public DiscordCoreAPI::DCAException {
		ConnectionError(const std::string&, ConnectionErrorType, std::source_location = std::source_location::current()) noexcept;

		ConnectionErrorType getType() noexcept;

	  protected:
		ConnectionErrorType errorType{};
	};

	std::string reportSSLError(const std::string& errorPosition, int32_t errorValue = 0, SSL* ssl = nullptr) noexcept;

	std::string reportError(const std::string& errorPosition) noexcept;

	struct DiscordCoreAPI_Dll PollFDWrapper {
		std::vector<uint32_t> indices{};
		std::vector<pollfd> polls{};
	};

#ifdef _WIN32
	struct DiscordCoreAPI_Dll WSADataWrapper {
		struct DiscordCoreAPI_Dll WSADataDeleter {
			void operator()(WSADATA* other) noexcept;
		};

		WSADataWrapper();

	  protected:
		std::unique_ptr<WSADATA, WSADataDeleter> ptr{ std::make_unique<WSADATA>().release(), WSADataDeleter{} };
	};
#endif

	struct DiscordCoreAPI_Dll SSL_CTXWrapper {
		struct DiscordCoreAPI_Dll SSL_CTXDeleter {
			void operator()(SSL_CTX* other) noexcept;
		};

		SSL_CTXWrapper& operator=(SSL_CTX* other) noexcept;

		operator SSL_CTX*() noexcept;

	  protected:
		std::unique_ptr<SSL_CTX, SSL_CTXDeleter> ptr{ nullptr, SSL_CTXDeleter{} };
	};

	struct DiscordCoreAPI_Dll SSLWrapper {
		struct DiscordCoreAPI_Dll SSLDeleter {
			void operator()(SSL* other) noexcept;
		};

		SSLWrapper& operator=(std::nullptr_t other) noexcept;

		SSLWrapper& operator=(SSL* other) noexcept;

		operator SSL*() noexcept;

	  protected:
		std::unique_ptr<SSL, SSLDeleter> ptr{ nullptr, SSLDeleter{} };
	};

	struct DiscordCoreAPI_Dll SOCKETWrapper {
		struct DiscordCoreAPI_Dll SOCKETDeleter {
			void operator()(SOCKET* other) noexcept;
		};
		SOCKETWrapper() noexcept = default;

		SOCKETWrapper& operator=(SOCKET other) noexcept;

		SOCKETWrapper(SOCKET other) noexcept;

		operator SOCKET() noexcept;

		~SOCKETWrapper() noexcept = default;

	  protected:
		std::unique_ptr<SOCKET, SOCKETDeleter> ptr{};
	};

	struct DiscordCoreAPI_Dll addrinfoWrapper {
		addrinfo* operator->() noexcept;

		operator addrinfo**() noexcept;

		operator addrinfo*() noexcept;

	  protected:
		addrinfo value{};
		addrinfo* ptr{ &value };
	};

	class DiscordCoreAPI_Dll SSLConnectionInterface {
	  public:
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

	class DiscordCoreAPI_Dll SSLDataInterface {
	  public:
		friend class HttpsClient;

		virtual void writeData(std::string_view dataToWrite, bool priority) = 0;

		virtual std::basic_string_view<uint8_t> getInputBuffer() noexcept = 0;

		virtual ~SSLDataInterface() noexcept = default;

	  protected:
		const uint64_t maxBufferSize{ (1024 * 16) };
		RingBuffer<uint8_t, 16> outputBuffer{};
		RingBuffer<uint8_t, 128> inputBuffer{};
		int64_t bytesRead{};
	};

	class DiscordCoreAPI_Dll TCPConnection : public SSLDataInterface, public SSLConnectionInterface {
	  public:
		TCPConnection() noexcept = default;

		TCPConnection(const std::string& baseUrlNew, const uint16_t portNew, bool doWePrintErrors, bool areWeAStandaloneSocket);

		template<typename ValueType> static std::unordered_map<uint32_t, ValueType*> processIO(std::unordered_map<uint32_t, ValueType*>& shardMap) {
			std::unordered_map<uint32_t, ValueType*> returnData{};
			PollFDWrapper readWriteSet{};
			for (auto& [key, value]: shardMap) {
				if (value && value->areWeStillConnected()) {
					pollfd fdSet{ .fd = static_cast<SOCKET>(value->socket) };
					if ((value->outputBuffer.getUsedSpace() > 0 || value->writeWantWrite || value->readWantWrite) && !value->readWantRead &&
						!value->writeWantRead) {
						fdSet.events = POLLIN | POLLOUT;
					} else if (!value->writeWantWrite && !value->readWantWrite) {
						fdSet.events = POLLIN;
					}
					readWriteSet.indices.emplace_back(key);
					readWriteSet.polls.emplace_back(fdSet);
				}
			}

			if (readWriteSet.polls.size() == 0) {
				return returnData;
			}
			if (auto returnDataNew = poll(readWriteSet.polls.data(), static_cast<u_long>(readWriteSet.polls.size()), 1);
				returnDataNew == SOCKET_ERROR) {
				bool didWeFindTheSocket{};
				for (size_t x = 0; x < readWriteSet.polls.size(); ++x) {
					if (readWriteSet.polls[x].revents & POLLERR || readWriteSet.polls[x].revents & POLLHUP ||
						readWriteSet.polls[x].revents & POLLNVAL) {
						returnData.emplace(readWriteSet.indices[x], shardMap[readWriteSet.indices[x]]);
						readWriteSet.indices.erase(readWriteSet.indices.begin() + x);
						readWriteSet.polls.erase(readWriteSet.polls.begin() + x);
						didWeFindTheSocket = true;
					}
				}
				if (!didWeFindTheSocket) {
					for (size_t x = 0; x < readWriteSet.polls.size(); ++x) {
						returnData.emplace(readWriteSet.indices[x], shardMap[readWriteSet.indices[x]]);
					}
					return returnData;
				}

			} else if (returnDataNew == 0) {
				return returnData;
			}
			for (size_t x = 0; x < readWriteSet.polls.size(); ++x) {
				if (readWriteSet.polls[x].revents & POLLOUT || (POLLIN && shardMap[readWriteSet.indices[x]]->writeWantRead)) {
					if (!shardMap[readWriteSet.indices[x]]->processWriteData()) {
						returnData.emplace(readWriteSet.indices[x], shardMap[readWriteSet.indices[x]]);
						continue;
					}
				}
				if (readWriteSet.polls[x].revents & POLLIN || (POLLOUT && shardMap[readWriteSet.indices[x]]->readWantWrite)) {
					if (!shardMap[readWriteSet.indices[x]]->processReadData()) {
						returnData.emplace(readWriteSet.indices[x], shardMap[readWriteSet.indices[x]]);
						continue;
					}
				}
				if (readWriteSet.polls[x].revents & POLLERR || readWriteSet.polls[x].revents & POLLNVAL || readWriteSet.polls[x].revents & POLLHUP) {
					returnData.emplace(readWriteSet.indices[x], shardMap[readWriteSet.indices[x]]);
					continue;
				}
			}
			return returnData;
		}

		void writeData(std::string_view dataToWrite, bool priority);

		std::basic_string_view<uint8_t> getInputBuffer() noexcept;

		virtual void handleBuffer() noexcept = 0;

		void processIO(int32_t waitTimeInMs);

		bool areWeStillConnected() noexcept;

		int64_t getBytesRead() noexcept;

		bool processWriteData();

		bool processReadData();

		void reset() noexcept;

		virtual ~TCPConnection() noexcept = default;

	  protected:
		inline static DiscordCoreAPI::StopWatch<std::chrono::milliseconds> stopWatch{ 10ms };
		bool areWeAStandaloneSocket{};
		bool doWePrintErrors{};
		bool writeWantWrite{};
		bool writeWantRead{};
		bool readWantWrite{};
		bool readWantRead{};
	};
}
