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
/// TCPConnection.hpp - Header file for the "Tcp connection" stuff.
/// Dec 12, 2021
/// https://discordcoreapi.com
/// \file TCPConnection.hpp
#pragma once

#include <discordcoreapi/Utilities/RingBuffer.hpp>
#include <discordcoreapi/Utilities/EventEntities.hpp>

#if !defined(OPENSSL_NO_DEPRECATED)
	#define OPENSSL_NO_DEPRECATED
#endif

#include <openssl/err.h>
#include <openssl/ssl.h>

#if defined(_WIN32)
	#if defined(EWOULDBLOCK)
		#undef EWOULDBLOCK
	#endif
	#if defined(SHUT_RDWR)
		#undef SHUT_RDWR
	#endif
	#if defined(pollfd)
		#undef pollfd
	#endif
	#if defined(connect)
		#undef connect
	#endif
	#if defined(errno)
		#undef errno
	#endif
	#if defined(close)
		#undef close
	#endif
	#if defined(poll)
		#undef poll
	#endif
	#define EWOULDBLOCK WSAEWOULDBLOCK
	#define SHUT_RDWR SD_BOTH
	#define pollfd WSAPOLLFD
	#define connect(x, y, z) WSAConnect(x, y, z, nullptr, nullptr, nullptr, nullptr)
	#define errno WSAGetLastError()
	#define close closesocket
	#define poll(x, y, z) WSAPoll(x, y, z)
	#pragma comment(lib, "Ws2_32.lib")
	#include <WinSock2.h>
	#include <ws2tcpip.h>
DCA_INLINE bool isValidSocket(SOCKET s) {
	return s != INVALID_SOCKET;
};

#else
using SOCKET = int32_t;
DCA_INLINE bool isValidSocket(SOCKET s) {
	return s >= 0;
};
	#include <netinet/tcp.h>
	#include <netinet/in.h>
	#include <sys/socket.h>
	#include <sys/types.h>
	#include <arpa/inet.h>
	#include <unistd.h>
	#include <errno.h>
	#include <netdb.h>
	#include <fcntl.h>
	#include <poll.h>
#endif

#if !defined(SOCKET_ERROR)
	#define SOCKET_ERROR SOCKET(-1)
#endif

#if !defined(INVALID_SOCKET)
	#define INVALID_SOCKET (-1)
#endif

namespace discord_core_api {

	namespace discord_core_internal {

		enum class connection_status {
			NO_Error		 = 0,
			CONNECTION_Error = 1,
			POLLHUP_Error	 = 2,
			POLLNVAL_Error	 = 3,
			POLLERR_Error	 = 4,
			READ_Error		 = 5,
			WRITE_Error		 = 6,
			SOCKET_Error	 = 7
		};

		DCA_INLINE jsonifier::string reportSSLError(jsonifier::string_view errorPosition, int32_t errorValue = 0, SSL* SSL = nullptr) {
			std::stringstream stream{};
			stream << errorPosition << " error: ";
			if (SSL) {
				stream << ERR_error_string(static_cast<unsigned long>(SSL_get_error(SSL, errorValue)), nullptr) << ", " << ERR_error_string(ERR_get_error(), nullptr);
			} else {
				stream << ERR_error_string(ERR_get_error(), nullptr);
			}
			return jsonifier::string{ stream.str() };
		}

		DCA_INLINE jsonifier::string reportError(jsonifier::string_view errorPosition) {
			std::stringstream stream{};
			stream << errorPosition << " error: ";
#if defined(_WIN32)
	#if defined(UNICODE)
			unique_ptr<wchar_t[]> string{ makeUnique<wchar_t[]>(1024) };
	#else
			unique_ptr<char[]> string{ makeUnique<char[]>(1024) };
	#endif
			FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, static_cast<DWORD>(WSAGetLastError()), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				static_cast<LPTSTR>(string.get()), 1024, nullptr);
			stream << WSAGetLastError() << ", " << string;
#else
			stream << strerror(errno);
#endif
			return jsonifier::string{ stream.str() };
		}

#if defined(_WIN32)
		struct wsadata_wrapper {
			struct wsadata_deleter {
				DCA_INLINE void operator()(WSADATA* other) {
					WSACleanup();
					delete other;
				}
			};

			DCA_INLINE wsadata_wrapper() {
				auto returnData = WSAStartup(MAKEWORD(2, 2), ptr.get());
				if (returnData) {
					message_printer::printError<print_message_type::general>(reportError("wsadata_wrapper::wsadata_wrapper()").data());
				}
			}

		  protected:
			unique_ptr<WSADATA, wsadata_deleter> ptr{ makeUnique<WSADATA, wsadata_deleter>() };
		};
#endif

		struct poll_fd_wrapper {
			jsonifier::vector<uint64_t> indices{};
			jsonifier::vector<pollfd> polls{};
		};

		struct ssl_ctx_wrapper {
			struct ssl_ctx_deleter {
				DCA_INLINE void operator()(SSL_CTX* other) {
					if (other) {
						SSL_CTX_free(other);
						other = nullptr;
					}
				}
			};

			DCA_INLINE ssl_ctx_wrapper& operator=(SSL_CTX* other) {
				ptr.reset(other);
				return *this;
			}

			DCA_INLINE operator SSL_CTX*() {
				return ptr.get();
			}

		  protected:
			unique_ptr<SSL_CTX, ssl_ctx_deleter> ptr{};
		};

		class ssl_wrapper {
		  public:
			struct ssl_deleter {
				DCA_INLINE void operator()(SSL* other) {
					if (other) {
						SSL_shutdown(other);
						SSL_free(other);
						other = nullptr;
					}
				}
			};

			DCA_INLINE ssl_wrapper() = default;

			DCA_INLINE ssl_wrapper& operator=(ssl_wrapper&& other) noexcept {
				ptr = std::move(other.ptr);
				return *this;
			}

			DCA_INLINE ssl_wrapper(ssl_wrapper&& other) noexcept {
				*this = std::move(other);
			}

			DCA_INLINE ssl_wrapper& operator=(SSL* other) {
				ptr.reset(other);
				return *this;
			}

			DCA_INLINE explicit operator bool() {
				return ptr.operator bool();
			}

			DCA_INLINE operator SSL*() {
				return ptr.get();
			}

		  protected:
			unique_ptr<SSL, ssl_deleter> ptr{};
		};

		class socket_wrapper {
		  public:
			struct socket_deleter {
				DCA_INLINE void operator()(SOCKET* ptrNew) {
					if (ptrNew && *ptrNew != INVALID_SOCKET) {
						shutdown(*ptrNew, SHUT_RDWR);
						close(*ptrNew);
						*ptrNew = INVALID_SOCKET;
						delete ptrNew;
						ptrNew = nullptr;
					};
				}
			};

			DCA_INLINE socket_wrapper() = default;

			DCA_INLINE socket_wrapper& operator=(socket_wrapper&& other) noexcept {
				ptr = std::move(other.ptr);
				return *this;
			}

			DCA_INLINE socket_wrapper(socket_wrapper&& other) noexcept {
				*this = std::move(other);
			}

			DCA_INLINE socket_wrapper& operator=(SOCKET other) {
				ptr.reset(new SOCKET{ other });
				return *this;
			}

			DCA_INLINE socket_wrapper(SOCKET other) {
				*this = other;
			}

			DCA_INLINE explicit operator bool() {
				return ptr.operator bool();
			}

			DCA_INLINE operator SOCKET() {
				if (ptr.operator bool()) {
					return *ptr;
				} else {
					return INVALID_SOCKET;
				}
			}

		  protected:
			unique_ptr<SOCKET, socket_deleter> ptr{};
		};

		struct addrinfo_wrapper {
			DCA_INLINE addrinfo* operator->() {
				return ptr;
			}

			DCA_INLINE operator addrinfo**() {
				return &ptr;
			}

			DCA_INLINE operator addrinfo*() {
				return ptr;
			}

		  protected:
			addrinfo value{};
			addrinfo* ptr{ &value };
		};

		class ssl_context_holder {
		  public:
			DCA_INLINE static ssl_ctx_wrapper context{};
			DCA_INLINE static std::mutex accessMutex{};

			DCA_INLINE static bool initialize() {
				if (ssl_context_holder::context = SSL_CTX_new(TLS_client_method()); !ssl_context_holder::context) {
					return false;
				}

				if (!SSL_CTX_set_min_proto_version(ssl_context_holder::context, TLS1_2_VERSION)) {
					return false;
				}

#if defined(SSL_OP_IGNORE_UNEXPECTED_EOF)
				auto originalOptions{ SSL_CTX_get_options(ssl_context_holder::context) | SSL_OP_IGNORE_UNEXPECTED_EOF };
				if (SSL_CTX_set_options(ssl_context_holder::context, SSL_OP_IGNORE_UNEXPECTED_EOF) != originalOptions) {
					return false;
				}
#endif
				return true;
			}
		};

		template<typename value_type> class ssl_data_interface {
		  public:
			template<typename value_type2> friend class tcp_connection;
			friend class https_client;

			ssl_data_interface& operator=(ssl_data_interface<value_type>&& other) noexcept {
				outputBuffer = std::move(other.outputBuffer);
				inputBuffer	 = std::move(other.inputBuffer);
				bytesRead	 = other.bytesRead;
				return *this;
			}

			ssl_data_interface(ssl_data_interface<value_type>&& other) noexcept {
				*this = std::move(other);
			}

			template<typename value_type_new> DCA_INLINE void writeData(jsonifier::string_view_base<value_type_new> dataToWrite, bool priority) {
				if (static_cast<value_type*>(this)->areWeStillConnected()) {
					if (dataToWrite.size() > 0 && static_cast<value_type*>(this)->ssl) {
						if (priority && dataToWrite.size() < maxBufferSize) {
							outputBuffer.clear();
							outputBuffer.writeData(dataToWrite.data(), dataToWrite.size());
							static_cast<value_type*>(this)->processWriteData();
						} else {
							uint64_t remainingBytes{ dataToWrite.size() };
							while (remainingBytes > 0) {
								uint64_t amountToCollect{ dataToWrite.size() >= maxBufferSize ? maxBufferSize : dataToWrite.size() };
								outputBuffer.writeData(dataToWrite.data(), amountToCollect);
								dataToWrite	   = jsonifier::string_view_base{ dataToWrite.data() + amountToCollect, dataToWrite.size() - amountToCollect };
								remainingBytes = dataToWrite.size();
							}
						}
					}
					return;
				} else {
					return;
				}
			}

			DCA_INLINE auto getInputBuffer() {
				return inputBuffer.readData();
			}

			DCA_INLINE int64_t getBytesRead() {
				return bytesRead;
			}

			DCA_INLINE void reset() {
				outputBuffer.clear();
				inputBuffer.clear();
				bytesRead = 0;
			}

		  protected:
			const uint64_t maxBufferSize{ (1024 * 16) };
			ring_buffer<uint8_t, 16> outputBuffer{};
			ring_buffer<uint8_t, 64> inputBuffer{};
			int64_t bytesRead{};

			DCA_INLINE ssl_data_interface() = default;

			virtual ~ssl_data_interface() = default;
		};

		template<typename value_type> class tcp_connection : public ssl_data_interface<tcp_connection<value_type>> {
		  public:
			connection_status currentStatus{ connection_status::NO_Error };
			socket_wrapper socket{};
			bool writeWantWrite{};
			bool writeWantRead{};
			bool readWantWrite{};
			bool readWantRead{};
			ssl_wrapper ssl{};

			tcp_connection& operator=(tcp_connection&& other)	   = default;
			tcp_connection(tcp_connection&& other)				   = default;
			tcp_connection& operator=(const tcp_connection& other) = default;
			tcp_connection(const tcp_connection& other)			   = default;

			DCA_INLINE tcp_connection(const jsonifier::string& baseUrlNew, const uint16_t portNew) {
				jsonifier::string addressString{};
				auto httpsFind = baseUrlNew.find("https://");
				auto comFind   = baseUrlNew.find(".com");
				auto orgFind   = baseUrlNew.find(".org");
				if (httpsFind != jsonifier::string::npos && comFind != jsonifier::string::npos) {
					addressString = baseUrlNew.substr(httpsFind + jsonifier::string_view{ "https://" }.size(),
						comFind + jsonifier::string_view{ ".com" }.size() - jsonifier::string_view{ "https://" }.size());
				} else if (httpsFind != jsonifier::string::npos && orgFind != jsonifier::string::npos) {
					addressString = baseUrlNew.substr(httpsFind + jsonifier::string_view{ "https://" }.size(),
						orgFind + jsonifier::string_view{ ".org" }.size() - jsonifier::string_view{ "https://" }.size());
				} else {
					addressString = baseUrlNew;
				}
				addrinfo_wrapper hints{}, address{};
				hints->ai_family   = AF_INET;
				hints->ai_socktype = SOCK_STREAM;
				hints->ai_protocol = IPPROTO_TCP;

				if (getaddrinfo(addressString.data(), jsonifier::toString(portNew).data(), hints, address)) {
					message_printer::printError<print_message_type::general>(reportError("Tcp_connection::getaddrinfo(), to: " + baseUrlNew));
					currentStatus = connection_status::CONNECTION_Error;
					socket		  = INVALID_SOCKET;
					return;
				}

				if (socket = ::socket(address->ai_family, address->ai_socktype, address->ai_protocol); !isValidSocket(socket.operator SOCKET())) {
					message_printer::printError<print_message_type::general>(reportError("Tcp_connection::SOCKET(), to: " + baseUrlNew));
					currentStatus = connection_status::CONNECTION_Error;
					socket		  = INVALID_SOCKET;
					return;
				}

				if (::connect(socket, address->ai_addr, static_cast<int32_t>(address->ai_addrlen)) == SOCKET_ERROR) {
					message_printer::printError<print_message_type::general>(reportError("Tcp_connection::connect(), to: " + baseUrlNew));
					currentStatus = connection_status::CONNECTION_Error;
					socket		  = INVALID_SOCKET;
					return;
				}

				std::unique_lock lock{ ssl_context_holder::accessMutex };
				if (ssl = SSL_new(ssl_context_holder::context); !ssl) {
					message_printer::printError<print_message_type::general>(
						reportSSLError("Tcp_connection::connect::SSL_new(), to: " + baseUrlNew) + "\n" + reportError("Tcp_connection::connect::SSL_new(), to: " + baseUrlNew));
					currentStatus = connection_status::CONNECTION_Error;
					socket		  = INVALID_SOCKET;
					ssl			  = nullptr;
					return;
				}
				lock.unlock();

				if (auto result{ SSL_set_fd(ssl, static_cast<int32_t>(socket)) }; result != 1) {
					message_printer::printError<print_message_type::general>(reportSSLError("Tcp_connection::connect::SSL_set_fd(), to: " + baseUrlNew) + "\n" +
						reportError("Tcp_connection::connect::SSL_set_fd(), to: " + baseUrlNew));
					currentStatus = connection_status::CONNECTION_Error;
					socket		  = INVALID_SOCKET;
					ssl			  = nullptr;
					return;
				}

				/* sni */
				if (auto result{ SSL_set_tlsext_host_name(ssl, addressString.data()) }; result != 1) {
					message_printer::printError<print_message_type::general>(reportSSLError("Tcp_connection::connect::SSL_set_tlsext_host_name(), to: " + baseUrlNew) + "\n" +
						reportError("Tcp_connection::connect::SSL_set_tlsext_host_name(), to: " + baseUrlNew));
					currentStatus = connection_status::CONNECTION_Error;
					socket		  = INVALID_SOCKET;
					ssl			  = nullptr;
					return;
				}

				if (auto result{ SSL_connect(ssl) }; result != 1) {
					message_printer::printError<print_message_type::general>(reportSSLError("Tcp_connection::connect::SSL_connect(), to: " + baseUrlNew) + "\n" +
						reportError("Tcp_connection::connect::SSL_connect(), to: " + baseUrlNew));
					currentStatus = connection_status::CONNECTION_Error;
					socket		  = INVALID_SOCKET;
					ssl			  = nullptr;
					return;
				}

#if defined(_WIN32)
				u_long value02{ 1 };
				if (auto returnData{ ioctlsocket(socket, FIONBIO, &value02) }; returnData == SOCKET_ERROR) {
					message_printer::printError<print_message_type::general>(reportError("Tcp_connection::connect::ioctlsocket(), to: " + baseUrlNew));
					currentStatus = connection_status::CONNECTION_Error;
					socket		  = INVALID_SOCKET;
					ssl			  = nullptr;
					return;
				}
#else
				if (auto returnData{ fcntl(socket, F_SETFL, fcntl(socket, F_GETFL, 0) | O_NONBLOCK) }; returnData == SOCKET_ERROR) {
					message_printer::printError<print_message_type::general>(reportError("Tcp_connection::connect::fcntl(), to: " + baseUrlNew));
					currentStatus = connection_status::CONNECTION_Error;
					socket		  = INVALID_SOCKET;
					ssl			  = nullptr;
					return;
				}
#endif
				currentStatus = connection_status::NO_Error;
			}

			DCA_INLINE connection_status processIO(int32_t waitTimeInMs) {
				if (!areWeStillConnected()) {
					return currentStatus;
				};
				pollfd readWriteSet{};
				readWriteSet.fd = static_cast<SOCKET>(socket);
				if (writeWantRead || readWantRead) {
					readWriteSet.events = POLLIN;
				} else if (writeWantWrite || readWantWrite) {
					readWriteSet.events = POLLOUT;
				} else if (static_cast<value_type*>(this)->outputBuffer.getUsedSpace() > 0) {
					readWriteSet.events = POLLIN | POLLOUT;
				} else {
					readWriteSet.events = POLLIN;
				}
				if (auto returnValue = poll(&readWriteSet, 1, waitTimeInMs); returnValue == SOCKET_ERROR) {
					message_printer::printError<print_message_type::general>(
						reportSSLError("Tcp_connection::processIO() 00") + "\n" + reportError("Tcp_connection::processIO() 00"));
					socket		  = INVALID_SOCKET;
					ssl			  = nullptr;
					currentStatus = connection_status::SOCKET_Error;
					return currentStatus;
				} else if (returnValue == 0) {
					return currentStatus;
				} else {
					if (readWriteSet.revents & POLLOUT || (POLLIN && writeWantRead)) {
						if (!processWriteData()) {
							message_printer::printError<print_message_type::general>(
								reportSSLError("Tcp_connection::processIO() 01") + "\n" + reportError("Tcp_connection::processIO() 01"));
							currentStatus = connection_status::WRITE_Error;
							socket		  = INVALID_SOCKET;
							ssl			  = nullptr;
							return currentStatus;
						}
					}
					if (readWriteSet.revents & POLLIN || (POLLOUT && readWantWrite)) {
						if (!processReadData()) {
							message_printer::printError<print_message_type::general>(
								reportSSLError("Tcp_connection::processIO() 02") + "\n" + reportError("Tcp_connection::processIO() 02"));
							currentStatus = connection_status::READ_Error;
							socket		  = INVALID_SOCKET;
							ssl			  = nullptr;
							return currentStatus;
						}
					}
					if (readWriteSet.revents & POLLERR) {
						message_printer::printError<print_message_type::general>(
							reportSSLError("Tcp_connection::processIO() 03") + "\n" + reportError("Tcp_connection::processIO() 03"));
						currentStatus = connection_status::POLLERR_Error;
						socket		  = INVALID_SOCKET;
						ssl			  = nullptr;
					}
					if (readWriteSet.revents & POLLNVAL) {
						message_printer::printError<print_message_type::general>(
							reportSSLError("Tcp_connection::processIO() 04") + "\n" + reportError("Tcp_connection::processIO() 04"));
						currentStatus = connection_status::POLLNVAL_Error;
						socket		  = INVALID_SOCKET;
						ssl			  = nullptr;
					}
					if (readWriteSet.revents & POLLHUP) {
						currentStatus = connection_status::POLLHUP_Error;
						socket		  = INVALID_SOCKET;
						ssl			  = nullptr;
					}
				}
				return currentStatus;
			}

			DCA_INLINE bool areWeStillConnected() {
				if (socket.operator bool() && socket.operator SOCKET() != INVALID_SOCKET && currentStatus == connection_status::NO_Error && ssl.operator bool()) {
					pollfd fdEvent = {};
					fdEvent.fd	   = socket;
					fdEvent.events = POLLOUT;
					int32_t result = poll(&fdEvent, 1, 1);
					if (result == SOCKET_ERROR || fdEvent.revents & POLLHUP || fdEvent.revents & POLLNVAL || fdEvent.revents & POLLERR) {
						socket = INVALID_SOCKET;
						ssl	   = nullptr;
						return false;
					}
					return true;
				} else {
					return false;
				}
			}

			DCA_INLINE bool processWriteData() {
				writeWantRead  = false;
				writeWantWrite = false;
				if (static_cast<value_type*>(this)->outputBuffer.getUsedSpace() > 0 && areWeStillConnected()) {
					uint64_t bytesToWrite{ static_cast<value_type*>(this)->outputBuffer.getCurrentTail()->getUsedSpace() };

					size_t writtenBytes{};
					auto returnData{ SSL_write_ex(ssl, static_cast<value_type*>(this)->outputBuffer.readData().data(), bytesToWrite, &writtenBytes) };
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
							ssl	   = nullptr;
							return false;
						}
						default: {
							return false;
						}
					}
				}
				return true;
			}

			DCA_INLINE bool processReadData() {
				readWantRead  = false;
				readWantWrite = false;
				if (!static_cast<value_type*>(this)->inputBuffer.isItFull() && areWeStillConnected()) {
					do {
						size_t readBytes{};
						uint64_t bytesToRead{ static_cast<value_type*>(this)->maxBufferSize };
						auto returnData{ SSL_read_ex(ssl, static_cast<value_type*>(this)->inputBuffer.getCurrentHead()->getCurrentHead(), bytesToRead, &readBytes) };
						auto errorValue{ SSL_get_error(ssl, returnData) };
						if (static_cast<int64_t>(readBytes) > 0) {
							static_cast<value_type*>(this)->inputBuffer.getCurrentHead()->modifyReadOrWritePosition(ring_buffer_access_type::write, readBytes);
							static_cast<value_type*>(this)->inputBuffer.modifyReadOrWritePosition(ring_buffer_access_type::write, 1);
							static_cast<value_type*>(this)->bytesRead += readBytes;
							static_cast<value_type*>(this)->handleBuffer();
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
								ssl	   = nullptr;
								return true;
							}
							default: {
								return false;
							}
						}
					} while (areWeStillConnected() && SSL_pending(ssl) && !static_cast<value_type*>(this)->inputBuffer.isItFull() && !readWantRead);
				}
				return true;
			}

			template<typename value_type2> DCA_INLINE static unordered_map<uint64_t, value_type2*> processIO(unordered_map<uint64_t, value_type2*>& shardMap) {
				unordered_map<uint64_t, value_type2*> returnData{};
				poll_fd_wrapper readWriteSet{};
				for (auto& [key, value]: shardMap) {
					if (value->areWeStillConnected()) {
						pollfd fdSet{};
						fdSet.fd = static_cast<SOCKET>(value->socket);
						if (value->writeWantRead || value->readWantRead) {
							fdSet.events = POLLIN;
						} else if (value->writeWantWrite || value->readWantWrite) {
							fdSet.events = POLLOUT;
						} else if (value->outputBuffer.getUsedSpace() > 0) {
							fdSet.events = POLLIN | POLLOUT;
						} else {
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
				if (auto returnDataNew = poll(readWriteSet.polls.data(), static_cast<u_long>(readWriteSet.polls.size()), 1); returnDataNew == SOCKET_ERROR) {
					bool didWeFindTheSocket{};
					for (uint64_t x = 0; x < readWriteSet.polls.size(); ++x) {
						if (readWriteSet.polls.at(x).revents & POLLERR || readWriteSet.polls.at(x).revents & POLLHUP || readWriteSet.polls.at(x).revents & POLLNVAL) {
							shardMap.at(readWriteSet.indices.at(x))->currentStatus = connection_status::SOCKET_Error;
							returnData.emplace(readWriteSet.indices.at(x), shardMap.at(readWriteSet.indices.at(x)));
							readWriteSet.indices.erase(readWriteSet.indices.begin() + static_cast<int64_t>(x));
							readWriteSet.polls.erase(readWriteSet.polls.begin() + static_cast<int64_t>(x));
							didWeFindTheSocket = true;
						}
					}
					if (!didWeFindTheSocket) {
						for (uint64_t x = 0; x < readWriteSet.polls.size(); ++x) {
							shardMap.at(readWriteSet.indices.at(x))->currentStatus = connection_status::SOCKET_Error;
							returnData.emplace(readWriteSet.indices.at(x), shardMap.at(readWriteSet.indices.at(x)));
						}
						return returnData;
					}

				} else if (returnDataNew == 0) {
					return returnData;
				}
				for (uint64_t x = 0; x < readWriteSet.polls.size(); ++x) {
					if (readWriteSet.polls.at(x).revents & POLLOUT || (POLLIN && shardMap.at(readWriteSet.indices.at(x))->writeWantRead)) {
						if (!shardMap.at(readWriteSet.indices.at(x))->processWriteData()) {
							shardMap.at(readWriteSet.indices.at(x))->currentStatus = connection_status::WRITE_Error;
							returnData.emplace(readWriteSet.indices.at(x), shardMap.at(readWriteSet.indices.at(x)));
							continue;
						}
					}
					if (readWriteSet.polls.at(x).revents & POLLIN || (POLLOUT && shardMap.at(readWriteSet.indices.at(x))->readWantWrite)) {
						if (!shardMap.at(readWriteSet.indices.at(x))->processReadData()) {
							shardMap.at(readWriteSet.indices.at(x))->currentStatus = connection_status::READ_Error;
							returnData.emplace(readWriteSet.indices.at(x), shardMap.at(readWriteSet.indices.at(x)));
							continue;
						}
					}
					if (readWriteSet.polls.at(x).revents & POLLERR) {
						shardMap.at(readWriteSet.indices.at(x))->currentStatus = connection_status::POLLERR_Error;
						shardMap.at(readWriteSet.indices.at(x))->socket		   = INVALID_SOCKET;
						shardMap.at(readWriteSet.indices.at(x))->ssl		   = nullptr;
						returnData.emplace(readWriteSet.indices.at(x), shardMap.at(readWriteSet.indices.at(x)));
						continue;
					}
					if (readWriteSet.polls.at(x).revents & POLLNVAL) {
						shardMap.at(readWriteSet.indices.at(x))->currentStatus = connection_status::POLLNVAL_Error;
						shardMap.at(readWriteSet.indices.at(x))->socket		   = INVALID_SOCKET;
						shardMap.at(readWriteSet.indices.at(x))->ssl		   = nullptr;
						returnData.emplace(readWriteSet.indices.at(x), shardMap.at(readWriteSet.indices.at(x)));
						continue;
					}
					if (readWriteSet.polls.at(x).revents & POLLHUP) {
						shardMap.at(readWriteSet.indices.at(x))->currentStatus = connection_status::POLLHUP_Error;
						shardMap.at(readWriteSet.indices.at(x))->socket		   = INVALID_SOCKET;
						shardMap.at(readWriteSet.indices.at(x))->ssl		   = nullptr;
						returnData.emplace(readWriteSet.indices.at(x), shardMap.at(readWriteSet.indices.at(x)));
						continue;
					}
				}
				return returnData;
			}

			virtual DCA_INLINE void handleBuffer() = 0;

			DCA_INLINE void disconnect() {
				currentStatus = connection_status::CONNECTION_Error;
				static_cast<value_type*>(this)->reset();
				socket = INVALID_SOCKET;
				ssl	   = nullptr;
			}

			virtual DCA_INLINE ~tcp_connection() = default;

		  protected:
			DCA_INLINE tcp_connection() = default;
		};
	}

#if defined(connect)
	#undef connect
#endif
}
