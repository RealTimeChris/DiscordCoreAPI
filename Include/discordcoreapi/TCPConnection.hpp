/*
	MIT License

	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2022, 2023 Chris M. (RealTimeChris)

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/
/// TCPConnection.hpp - Header file for the "TCP Connection" stuff.
/// Dec 12, 2021
/// https://discordcoreapi.com
/// \file TCPConnection.hpp

#pragma once

#include <discordcoreapi/Utilities/RingBuffer.hpp>
#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/Utilities/EventEntities.hpp>

#ifndef OPENSSL_NO_DEPRECATED
	#define OPENSSL_NO_DEPRECATED
#endif

#include <openssl/err.h>
#include <openssl/ssl.h>

#ifdef _WIN32
	#define connect(x, y, z) WSAConnect(x, y, z, nullptr, nullptr, nullptr, nullptr)
	#ifdef errno
		#undef errno
		#define errno WSAGetLastError()
	#endif
	#ifdef EWOULDBLOCK
		#undef EWOULDBLOCK
		#define EWOULDBLOCK WSAEWOULDBLOCK
	#endif
	#define close closesocket
	#define SHUT_RDWR SD_BOTH
	#pragma comment(lib, "Ws2_32.lib")
	#include <WinSock2.h>
	#include <WS2tcpip.h>
	#define poll(x, y, z) WSAPoll(x, y, z)
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
using SOCKET = int32_t;
#endif

#ifndef SOCKET_ERROR
	#define SOCKET_ERROR SOCKET(-1)
#endif

#ifndef INVALID_SOCKET
	#define INVALID_SOCKET SOCKET(~0)
#endif
namespace DiscordCoreAPI {
	namespace DiscordCoreInternal {

		enum class ConnectionStatus {
			NO_Error = 0,
			CONNECTION_Error = 1,
			POLLHUP_Error = 2,
			POLLNVAL_Error = 3,
			POLLERR_Error = 4,
			READ_Error = 5,
			WRITE_Error = 6,
			SOCKET_Error = 7
		};

		inline std::string reportSSLError(const std::string& errorPosition, int32_t errorValue = 0, SSL* ssl = nullptr) noexcept {
			std::stringstream stream{};
			stream << errorPosition << " Error: ";
			if (ssl) {
				stream << ERR_error_string(SSL_get_error(ssl, errorValue), nullptr) << ", " << ERR_error_string(ERR_get_error(), nullptr) << std::endl
					   << std::endl;
			} else {
				stream << ERR_error_string(ERR_get_error(), nullptr) << std::endl << std::endl;
			}
			return stream.str();
		}

		inline std::string reportError(const std::string& errorPosition) noexcept {
			std::stringstream stream{};
			stream << errorPosition << " Error: ";
#ifdef _WIN32
			char string[1024]{};
			FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, WSAGetLastError(),
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), static_cast<LPTSTR>(string), 1024, nullptr);
			stream << WSAGetLastError() << ", " << string << std::endl;
#else
			stream << strerror(errno) << std::endl;
#endif
			return stream.str();
		}

#ifdef _WIN32
		struct WSADataWrapper {
			struct WSADataDeleter {
				inline void operator()(WSADATA* other) noexcept {
					WSACleanup();
					delete other;
				}
			};

			inline WSADataWrapper() {
				auto returnData = WSAStartup(MAKEWORD(2, 2), ptr.get());
				if (returnData) {
					MessagePrinter::printError<PrintMessageType::WebSocket>(reportError("WSADataWrapper::WSADataWrapper()"));
				}
			}

		  protected:
			UniquePtr<WSADATA, WSADataDeleter> ptr{ makeUnique<WSADATA, WSADataDeleter>() };
		};
#endif

		struct PollFDWrapper {
			std::vector<uint32_t> indices{};
			std::vector<pollfd> polls{};
		};

		struct SSL_CTXWrapper {
			struct SSL_CTXDeleter {
				inline void operator()(SSL_CTX* other) noexcept {
					if (other) {
						SSL_CTX_free(other);
						other = nullptr;
					}
				}
			};

			inline SSL_CTXWrapper& operator=(SSL_CTX* other) noexcept {
				ptr.reset(other);
				return *this;
			}

			inline operator SSL_CTX*() noexcept {
				return ptr.get();
			}

		  protected:
			UniquePtr<SSL_CTX, SSL_CTXDeleter> ptr{};
		};

		struct SSLWrapper {
			struct SSLDeleter {
				inline void operator()(SSL* other) noexcept {
					if (other) {
						SSL_shutdown(other);
						SSL_free(other);
						other = nullptr;
					}
				}
			};

			inline SSLWrapper& operator=(std::nullptr_t other) noexcept {
				if (ptr) {
					ptr.reset(other);
				}
				return *this;
			}

			inline SSLWrapper& operator=(SSL* other) noexcept {
				ptr.reset(other);
				return *this;
			}

			inline explicit operator bool() noexcept {
				return ptr;
			}

			inline operator SSL*() noexcept {
				return ptr.get();
			}

		  protected:
			UniquePtr<SSL, SSLDeleter> ptr{};
		};

		struct SOCKETWrapper {
			struct SOCKETDeleter {
				inline void operator()(SOCKET* ptr) noexcept {
					if (ptr && *ptr != INVALID_SOCKET) {
						shutdown(*ptr, SHUT_RDWR);
						close(*ptr);
						*ptr = INVALID_SOCKET;
						delete ptr;
						ptr = nullptr;
					};
				}
			};

			inline SOCKETWrapper() noexcept = default;

			inline SOCKETWrapper& operator=(SOCKET other) noexcept {
				ptr.reset(new SOCKET{ other });
				return *this;
			}

			inline SOCKETWrapper(SOCKET other) noexcept {
				*this = other;
			}

			inline explicit operator bool() noexcept {
				return ptr;
			}

			inline operator SOCKET() noexcept {
				if (ptr) {
					return *ptr;
				} else {
					return INVALID_SOCKET;
				}
			}

		  protected:
			UniquePtr<SOCKET, SOCKETDeleter> ptr{};
		};

		struct addrinfoWrapper {
			inline addrinfo* operator->() noexcept {
				return ptr;
			}

			inline operator addrinfo**() noexcept {
				return &ptr;
			}

			inline operator addrinfo*() noexcept {
				return ptr;
			}

		  protected:
			addrinfo value{};
			addrinfo* ptr{ &value };
		};

		class SSLContextHolder {
		  public:
			inline static SSL_CTXWrapper context{};
			inline static std::mutex accessMutex{};

			inline static bool initialize() noexcept {
				if (SSLContextHolder::context = SSL_CTX_new(TLS_client_method()); !SSLContextHolder::context) {
					return false;
				}

				if (!SSL_CTX_set_min_proto_version(SSLContextHolder::context, TLS1_2_VERSION)) {
					return false;
				}

#ifdef SSL_OP_IGNORE_UNEXPECTED_EOF
				auto originalOptions{ SSL_CTX_get_options(SSLContextHolder::context) | SSL_OP_IGNORE_UNEXPECTED_EOF };
				if (SSL_CTX_set_options(SSLContextHolder::context, SSL_OP_IGNORE_UNEXPECTED_EOF) != originalOptions) {
					return false;
				}
#endif
				return true;
			}
		};

		template<typename ValueType> class SSLDataInterface {
		  public:
			template<typename ValueType2> friend class TCPConnection;
			friend class HttpsClient;

			inline SSLDataInterface& operator=(SSLDataInterface&& other) noexcept {
				outputBuffer = std::move(other.outputBuffer);
				inputBuffer = std::move(other.inputBuffer);
				bytesRead = other.bytesRead;
				return *this;
			}
			inline SSLDataInterface(SSLDataInterface&& other) noexcept {
				*this = std::move(other);
			}

			inline void writeData(std::string_view dataToWrite, bool priority) {
				if (static_cast<ValueType*>(this)->areWeStillConnected()) {
					if (dataToWrite.size() > 0 && static_cast<ValueType*>(this)->ssl) {
						if (priority && dataToWrite.size() < maxBufferSize) {
							outputBuffer.clear();
							outputBuffer.writeData(reinterpret_cast<const uint8_t*>(dataToWrite.data()), dataToWrite.size());
							static_cast<ValueType*>(this)->processWriteData();
						} else {
							uint64_t remainingBytes{ dataToWrite.size() };
							while (remainingBytes > 0) {
								uint64_t amountToCollect{ dataToWrite.size() >= maxBufferSize ? maxBufferSize : dataToWrite.size() };
								outputBuffer.writeData(reinterpret_cast<const uint8_t*>(dataToWrite.data()), amountToCollect);
								dataToWrite = std::basic_string_view{ dataToWrite.data() + amountToCollect, dataToWrite.size() - amountToCollect };
								remainingBytes = dataToWrite.size();
							}
						}
					}
					return;
				} else {
					return;
				}
			}

			inline std::string getInputBuffer() noexcept {
				auto newReturnString{ inputBuffer.readData() };
				std::string newString{};
				newString.resize(newReturnString.size());
				std::memcpy(newString.data(), newReturnString.data(), newReturnString.size());
				return newString;
			}

			inline int64_t getBytesRead() noexcept {
				return bytesRead;
			}

			inline void reset() noexcept {
				outputBuffer.clear();
				inputBuffer.clear();
				bytesRead = 0;
			}

			virtual ~SSLDataInterface() noexcept = default;

		  protected:
			const uint64_t maxBufferSize{ (1024 * 16) };
			RingBuffer<uint8_t, 16> outputBuffer{};
			RingBuffer<uint8_t, 64> inputBuffer{};
			int64_t bytesRead{};

			inline SSLDataInterface() noexcept = default;
		};

		template<typename ValueType> class TCPConnection {
		  public:
			ConnectionStatus currentStatus{ ConnectionStatus::NO_Error };
			SOCKETWrapper socket{};
			bool writeWantWrite{};
			bool writeWantRead{};
			bool readWantWrite{};
			bool readWantRead{};
			SSLWrapper ssl{};

			inline TCPConnection& operator=(TCPConnection&& other) noexcept {
				writeWantWrite = other.writeWantWrite;
				currentStatus = other.currentStatus;
				readWantWrite = other.readWantWrite;
				writeWantRead = other.writeWantRead;
				readWantRead = other.readWantRead;
				socket = std::move(other.socket);
				ssl = std::move(other.ssl);
				return *this;
			}

			inline TCPConnection(TCPConnection&& other) noexcept {
				*this = std::move(other);
			}

			inline TCPConnection(const std::string& baseUrlNew, const uint16_t portNew) {
				std::string addressString{};
				auto httpsFind = baseUrlNew.find("https://");
				auto comFind = baseUrlNew.find(".com");
				auto orgFind = baseUrlNew.find(".org");
				if (httpsFind != std::string::npos && comFind != std::string::npos) {
					addressString = baseUrlNew.substr(httpsFind + std::string_view{ "https://" }.size(),
						comFind + std::string_view{ ".com" }.size() - std::string_view{ "https://" }.size());
				} else if (httpsFind != std::string::npos && orgFind != std::string::npos) {
					addressString = baseUrlNew.substr(httpsFind + std::string_view{ "https://" }.size(),
						orgFind + std::string_view{ ".org" }.size() - std::string_view{ "https://" }.size());
				} else {
					addressString = baseUrlNew;
				}
				addrinfoWrapper hints{}, address{};
				hints->ai_family = AF_INET;
				hints->ai_socktype = SOCK_STREAM;
				hints->ai_protocol = IPPROTO_TCP;

				if (getaddrinfo(addressString.c_str(), std::to_string(portNew).c_str(), hints, address)) {
					MessagePrinter::printError<PrintMessageType::WebSocket>(reportError("TCPConnection::getaddrinfo(), to: " + baseUrlNew));
					currentStatus = ConnectionStatus::CONNECTION_Error;
					socket = INVALID_SOCKET;
					return;
				}

				if (socket = ::socket(address->ai_family, address->ai_socktype, address->ai_protocol); socket.operator SOCKET() == INVALID_SOCKET) {
					MessagePrinter::printError<PrintMessageType::WebSocket>(reportError("TCPConnection::socket(), to: " + baseUrlNew));
					currentStatus = ConnectionStatus::CONNECTION_Error;
					socket = INVALID_SOCKET;
					return;
				}

				bool boolOptionVal{ true };
				if (setsockopt(socket, SOL_SOCKET, SO_KEEPALIVE, reinterpret_cast<const char*>(&boolOptionVal), sizeof(int32_t))) {
					MessagePrinter::printError<PrintMessageType::WebSocket>(reportError("TCPConnection::setsockopt(), to: " + baseUrlNew));
					currentStatus = ConnectionStatus::CONNECTION_Error;
					socket = INVALID_SOCKET;
					return;
				}

				if (::connect(socket, address->ai_addr, static_cast<int32_t>(address->ai_addrlen)) == SOCKET_ERROR) {
					MessagePrinter::printError<PrintMessageType::WebSocket>(reportError("TCPConnection::connect(), to: " + baseUrlNew));
					currentStatus = ConnectionStatus::CONNECTION_Error;
					socket = INVALID_SOCKET;
					return;
				}

				std::unique_lock lock{ SSLContextHolder::accessMutex };
				if (ssl = SSL_new(SSLContextHolder::context); !ssl) {
					MessagePrinter::printError<PrintMessageType::WebSocket>(reportSSLError("TCPConnection::connect::SSL_new(), to: " + baseUrlNew) +
						reportError("TCPConnection::connect::SSL_new(), to: " + baseUrlNew));
					currentStatus = ConnectionStatus::CONNECTION_Error;
					socket = INVALID_SOCKET;
					ssl = nullptr;
					return;
				}
				lock.unlock();

				if (auto result{ SSL_set_fd(ssl, socket) }; result != 1) {
					MessagePrinter::printError<PrintMessageType::WebSocket>(
						reportSSLError("TCPConnection::connect::SSL_set_fd(), to: " + baseUrlNew) +
						reportError("TCPConnection::connect::SSL_set_fd(), to: " + baseUrlNew));
					currentStatus = ConnectionStatus::CONNECTION_Error;
					socket = INVALID_SOCKET;
					ssl = nullptr;
					return;
				}

				/* SNI */
				if (auto result{ SSL_set_tlsext_host_name(ssl, addressString.c_str()) }; result != 1) {
					MessagePrinter::printError<PrintMessageType::WebSocket>(
						reportSSLError("TCPConnection::connect::SSL_set_tlsext_host_name(), to: " + baseUrlNew) +
						reportError("TCPConnection::connect::SSL_set_tlsext_host_name(), to: " + baseUrlNew));
					currentStatus = ConnectionStatus::CONNECTION_Error;
					socket = INVALID_SOCKET;
					ssl = nullptr;
					return;
				}

				if (auto result{ SSL_connect(ssl) }; result != 1) {
					MessagePrinter::printError<PrintMessageType::WebSocket>(
						reportSSLError("TCPConnection::connect::SSL_connect(), to: " + baseUrlNew) +
						reportError("TCPConnection::connect::SSL_connect(), to: " + baseUrlNew));
					currentStatus = ConnectionStatus::CONNECTION_Error;
					socket = INVALID_SOCKET;
					ssl = nullptr;
					return;
				}

#ifdef _WIN32
				u_long value02{ 1 };
				if (auto returnData{ ioctlsocket(socket, FIONBIO, &value02) }; returnData == SOCKET_ERROR) {
					MessagePrinter::printError<PrintMessageType::WebSocket>(reportError("TCPConnection::connect::ioctlsocket(), to: " + baseUrlNew));
					currentStatus = ConnectionStatus::CONNECTION_Error;
					socket = INVALID_SOCKET;
					ssl = nullptr;
					return;
				}
#else
				if (auto returnData{ fcntl(socket, F_SETFL, fcntl(socket, F_GETFL, 0) | O_NONBLOCK) }; returnData == SOCKET_ERROR) {
					MessagePrinter::printError<PrintMessageType::WebSocket>(reportError("TCPConnection::connect::fcntl(), to: " + baseUrlNew));
					currentStatus = ConnectionStatus::CONNECTION_Error;
					socket = INVALID_SOCKET;
					ssl = nullptr;
					return;
				}
#endif
				currentStatus = ConnectionStatus::NO_Error;
			}

			inline ConnectionStatus processIO(int32_t waitTimeInMs) {
				if (!areWeStillConnected()) {
					return currentStatus;
				};
				pollfd readWriteSet{ .fd = static_cast<SOCKET>(socket) };
				if ((static_cast<ValueType*>(this)->outputBuffer.getUsedSpace() > 0 || writeWantWrite || readWantWrite) && !readWantRead &&
					!writeWantRead) {
					readWriteSet.events = POLLIN | POLLOUT;
				} else if (!writeWantWrite && !readWantWrite) {
					readWriteSet.events = POLLIN;
				}
				if (auto returnValue = poll(&readWriteSet, 1, waitTimeInMs); returnValue == SOCKET_ERROR) {
					MessagePrinter::printError<PrintMessageType::WebSocket>(
						reportSSLError("TCPConnection::processIO() 00") + reportError("TCPConnection::processIO() 00"));
					socket = INVALID_SOCKET;
					ssl = nullptr;
					currentStatus = ConnectionStatus::SOCKET_Error;
					return currentStatus;
				} else if (returnValue == 0) {
					return currentStatus;
				} else {
					if (readWriteSet.revents & POLLOUT || (POLLIN && writeWantRead)) {
						if (!processWriteData()) {
							MessagePrinter::printError<PrintMessageType::WebSocket>(
								reportSSLError("TCPConnection::processIO() 01") + reportError("TCPConnection::processIO() 01"));
							currentStatus = ConnectionStatus::WRITE_Error;
							socket = INVALID_SOCKET;
							ssl = nullptr;
							return currentStatus;
						}
					}
					if (readWriteSet.revents & POLLIN || (POLLOUT && readWantWrite)) {
						if (!processReadData()) {
							MessagePrinter::printError<PrintMessageType::WebSocket>(
								reportSSLError("TCPConnection::processIO() 02") + reportError("TCPConnection::processIO() 02"));
							currentStatus = ConnectionStatus::READ_Error;
							socket = INVALID_SOCKET;
							ssl = nullptr;
							return currentStatus;
						}
					}
					if (readWriteSet.revents & POLLERR) {
						MessagePrinter::printError<PrintMessageType::WebSocket>(
							reportSSLError("TCPConnection::processIO() 03") + reportError("TCPConnection::processIO() 03"));
						currentStatus = ConnectionStatus::POLLERR_Error;
						socket = INVALID_SOCKET;
						ssl = nullptr;
					}
					if (readWriteSet.revents & POLLNVAL) {
						MessagePrinter::printError<PrintMessageType::WebSocket>(
							reportSSLError("TCPConnection::processIO() 04") + reportError("TCPConnection::processIO() 04"));
						currentStatus = ConnectionStatus::POLLNVAL_Error;
						socket = INVALID_SOCKET;
						ssl = nullptr;
					}
					if (readWriteSet.revents & POLLHUP) {
						currentStatus = ConnectionStatus::POLLHUP_Error;
						socket = INVALID_SOCKET;
						ssl = nullptr;
					}
				}
				return currentStatus;
			}

			inline bool areWeStillConnected() noexcept {
				if (socket.operator bool() && socket.operator SOCKET() != INVALID_SOCKET && currentStatus == ConnectionStatus::NO_Error &&
					ssl.operator bool()) {
					return true;
				} else {
					return false;
				}
			}

			inline bool processWriteData() {
				writeWantRead = false;
				writeWantWrite = false;
				if (static_cast<ValueType*>(this)->outputBuffer.getUsedSpace() > 0 && areWeStillConnected()) {
					uint64_t bytesToWrite{ static_cast<ValueType*>(this)->outputBuffer.getCurrentTail()->getUsedSpace() };

					uint64_t writtenBytes{};
					auto returnData{ SSL_write_ex(ssl, static_cast<ValueType*>(this)->outputBuffer.readData().data(), bytesToWrite, &writtenBytes) };
					auto errorValue{ SSL_get_error(ssl, returnData) };
					switch (errorValue) {
						case SSL_ERROR_WANT_READ: {
							writeWantRead = true;
							return true;
						}
						case SSL_ERROR_WANT_WRITE: {
							writeWantWrite = true;
							return true;
						}
						case SSL_ERROR_NONE: {
							return true;
						}
						case SSL_ERROR_ZERO_RETURN: {
							socket = INVALID_SOCKET;
							ssl = nullptr;
							return false;
						}
						default: {
							return false;
						}
					}
				}
				return true;
			}

			inline bool processReadData() {
				readWantRead = false;
				readWantWrite = false;
				if (!static_cast<ValueType*>(this)->inputBuffer.isItFull() && areWeStillConnected()) {
					do {
						uint64_t readBytes{};
						uint64_t bytesToRead{ static_cast<ValueType*>(this)->maxBufferSize };
						auto returnData{ SSL_read_ex(ssl, static_cast<ValueType*>(this)->inputBuffer.getCurrentHead()->getCurrentHead(), bytesToRead,
							&readBytes) };
						auto errorValue{ SSL_get_error(ssl, returnData) };
						if (static_cast<int64_t>(readBytes) > 0) {
							static_cast<ValueType*>(this)->inputBuffer.getCurrentHead()->modifyReadOrWritePosition(RingBufferAccessType::Write,
								readBytes);
							static_cast<ValueType*>(this)->inputBuffer.modifyReadOrWritePosition(RingBufferAccessType::Write, 1);
							static_cast<ValueType*>(this)->bytesRead += readBytes;
							static_cast<ValueType*>(this)->handleBuffer();
						}
						switch (errorValue) {
							case SSL_ERROR_WANT_READ: {
								readWantRead = true;
								return true;
							}
							case SSL_ERROR_WANT_WRITE: {
								readWantWrite = true;
								return true;
							}
							case SSL_ERROR_NONE: {
								break;
							}
							case SSL_ERROR_ZERO_RETURN: {
								socket = INVALID_SOCKET;
								ssl = nullptr;
								return true;
							}
							default: {
								return false;
							}
						}
					} while (areWeStillConnected() && SSL_pending(ssl) && !static_cast<ValueType*>(this)->inputBuffer.isItFull() && !readWantRead);
				}
				return true;
			}

			template<typename ValueType2>
			inline static std::unordered_map<uint32_t, ValueType2*> processIO(std::unordered_map<uint32_t, ValueType2*>& shardMap) {
				std::unordered_map<uint32_t, ValueType2*> returnData{};
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
					} else {
						returnData.emplace(key, value);
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
							shardMap[readWriteSet.indices[x]]->currentStatus = ConnectionStatus::SOCKET_Error;
							returnData.emplace(readWriteSet.indices[x], shardMap[readWriteSet.indices[x]]);
							readWriteSet.indices.erase(readWriteSet.indices.begin() + x);
							readWriteSet.polls.erase(readWriteSet.polls.begin() + x);
							didWeFindTheSocket = true;
						}
					}
					if (!didWeFindTheSocket) {
						for (size_t x = 0; x < readWriteSet.polls.size(); ++x) {
							shardMap[readWriteSet.indices[x]]->currentStatus = ConnectionStatus::SOCKET_Error;
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
							shardMap[readWriteSet.indices[x]]->currentStatus = ConnectionStatus::WRITE_Error;
							returnData.emplace(readWriteSet.indices[x], shardMap[readWriteSet.indices[x]]);
							continue;
						}
					}
					if (readWriteSet.polls[x].revents & POLLIN || (POLLOUT && shardMap[readWriteSet.indices[x]]->readWantWrite)) {
						if (!shardMap[readWriteSet.indices[x]]->processReadData()) {
							shardMap[readWriteSet.indices[x]]->currentStatus = ConnectionStatus::READ_Error;
							returnData.emplace(readWriteSet.indices[x], shardMap[readWriteSet.indices[x]]);
							continue;
						}
					}
					if (readWriteSet.polls[x].revents & POLLERR) {
						shardMap[readWriteSet.indices[x]]->currentStatus = ConnectionStatus::POLLERR_Error;
						shardMap[readWriteSet.indices[x]]->socket = INVALID_SOCKET;
						shardMap[readWriteSet.indices[x]]->ssl = nullptr;
						returnData.emplace(readWriteSet.indices[x], shardMap[readWriteSet.indices[x]]);
						continue;
					}
					if (readWriteSet.polls[x].revents & POLLNVAL) {
						shardMap[readWriteSet.indices[x]]->currentStatus = ConnectionStatus::POLLNVAL_Error;
						shardMap[readWriteSet.indices[x]]->socket = INVALID_SOCKET;
						shardMap[readWriteSet.indices[x]]->ssl = nullptr;
						returnData.emplace(readWriteSet.indices[x], shardMap[readWriteSet.indices[x]]);
						continue;
					}
					if (readWriteSet.polls[x].revents & POLLHUP) {
						shardMap[readWriteSet.indices[x]]->currentStatus = ConnectionStatus::POLLHUP_Error;
						shardMap[readWriteSet.indices[x]]->socket = INVALID_SOCKET;
						shardMap[readWriteSet.indices[x]]->ssl = nullptr;
						returnData.emplace(readWriteSet.indices[x], shardMap[readWriteSet.indices[x]]);
						continue;
					}
				}
				return returnData;
			}

			inline virtual ~TCPConnection() noexcept = default;

		  protected:
			inline TCPConnection() noexcept = default;
		};
	}

#ifdef connect
	#undef connect
#endif
}
