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
/// TCPConnection.cpp - Source file for the "TCP Connection" stuff.
/// Dec 12, 2021
/// https://discordcoreapi.com
/// \file TCPConnection.cpp

#include <discordcoreapi/TCPConnection.hpp>
#include <discordcoreapi/WebSocketEntities.hpp>

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
#endif

namespace DiscordCoreInternal {

	ConnectionError::ConnectionError(const std::string& stringError, ConnectionErrorType typeNew, std::source_location location) noexcept
		: DCAException{ stringError, location } {
		errorType = typeNew;
	}

	ConnectionErrorType ConnectionError::getType() noexcept {
		return errorType;
	}

	std::string reportSSLError(const std::string& errorPosition, int32_t errorValue, SSL* ssl) noexcept {
		std::stringstream stream{};
		stream << DiscordCoreAPI::shiftToBrightRed() << errorPosition << " Error: ";
		if (ssl) {
			stream << ERR_error_string(SSL_get_error(ssl, errorValue), nullptr) << ", " << ERR_error_string(ERR_get_error(), nullptr)
				   << DiscordCoreAPI::reset() << endl
				   << endl;
		} else {
			stream << ERR_error_string(ERR_get_error(), nullptr) << DiscordCoreAPI::reset() << endl << endl;
		}
		return stream.str();
	}

	std::string reportError(const std::string& errorPosition) noexcept {
		std::stringstream stream{};
		stream << DiscordCoreAPI::shiftToBrightRed() << errorPosition << " Error: ";
#ifdef _WIN32
		char string[1024]{};
	#ifdef UWP
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			( LPWSTR )string, 1024, NULL);
	#else
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			string, 1024, NULL);
	#endif
		stream << WSAGetLastError() << ", " << string << DiscordCoreAPI::reset() << endl;
#else
		stream << strerror(errno) << DiscordCoreAPI::reset();
#endif
		return stream.str();
	}

#ifdef _WIN32
	void WSADataWrapper::WSADataDeleter::operator()(WSADATA* other) noexcept {
		WSACleanup();
		delete other;
	}

	WSADataWrapper::WSADataWrapper() {
		auto returnData = WSAStartup(MAKEWORD(2, 2), ptr.get());
		if (returnData) {
			throw DiscordCoreAPI::DCAException{ reportError("WSADataWrapper::WSADataWrapper()") };
		}
	}
#endif

	void SSL_CTXWrapper::SSL_CTXDeleter::operator()(SSL_CTX* other) noexcept {
		if (other) {
			SSL_CTX_free(other);
			other = nullptr;
		}
	}

	SSL_CTXWrapper& SSL_CTXWrapper::operator=(SSL_CTX* other) noexcept {
		if (ptr) {
			ptr.reset(nullptr);
		}
		ptr = std::unique_ptr<SSL_CTX, SSL_CTXDeleter>(other, SSL_CTXDeleter{});
		return *this;
	}

	SSL_CTXWrapper::operator SSL_CTX*() noexcept {
		return ptr.get();
	}

	void SSLWrapper::SSLDeleter::operator()(SSL* other) noexcept {
		if (other) {
			SSL_shutdown(other);
			SSL_free(other);
			other = nullptr;
		}
	}

	SSLWrapper& SSLWrapper::operator=(std::nullptr_t other) noexcept {
		if (ptr) {
			ptr.reset(other);
		}
		return *this;
	}

	SSLWrapper& SSLWrapper::operator=(SSL* other) noexcept {
		if (ptr) {
			ptr.reset(nullptr);
		}
		ptr = std::unique_ptr<SSL, SSLDeleter>(other, SSLDeleter{});
		return *this;
	}

	SSLWrapper::operator SSL*() noexcept {
		return ptr.get();
	}

	SOCKETWrapper& SOCKETWrapper::operator=(SOCKET other) noexcept {
		ptr.reset(new SOCKET{ other });
		return *this;
	}

	SOCKETWrapper::SOCKETWrapper(SOCKET other) noexcept {
		*this = other;
	}

	void SOCKETWrapper::SOCKETDeleter::operator()(SOCKET* ptr) noexcept {
		if (ptr && *ptr != INVALID_SOCKET) {
			shutdown(*ptr, SHUT_RDWR);
			close(*ptr);
			*ptr = INVALID_SOCKET;
		};
	}

	SOCKETWrapper::operator SOCKET() noexcept {
		if (ptr) {
			return *ptr;
		} else {
			return INVALID_SOCKET;
		}
	}

	addrinfo* addrinfoWrapper::operator->() noexcept {
		return ptr;
	}

	addrinfoWrapper::operator addrinfo**() noexcept {
		return &ptr;
	}

	addrinfoWrapper::operator addrinfo*() noexcept {
		return ptr;
	}

	bool SSLConnectionInterface::initialize() noexcept {
		if (SSLConnectionInterface::context = SSL_CTX_new(TLS_client_method()); !SSLConnectionInterface::context) {
			return false;
		}

		if (!SSL_CTX_set_min_proto_version(SSLConnectionInterface::context, TLS1_2_VERSION)) {
			return false;
		}

#ifdef SSL_OP_IGNORE_UNEXPECTED_EOF
		auto originalOptions{ SSL_CTX_get_options(SSLConnectionInterface::context) | SSL_OP_IGNORE_UNEXPECTED_EOF };
		if (SSL_CTX_set_options(SSLConnectionInterface::context, SSL_OP_IGNORE_UNEXPECTED_EOF) != originalOptions) {
			return false;
		}
#endif
		return true;
	}

	TCPConnection::TCPConnection(const std::string& baseUrlNew, const uint16_t portNew, bool doWePrintErrorsNew, bool areWeAStandaloneSocketNew) {
		areWeAStandaloneSocket = areWeAStandaloneSocketNew;
		doWePrintErrors = doWePrintErrorsNew;
		std::string addressString{};
		auto httpsFind = baseUrlNew.find("https://");
		auto comFind = baseUrlNew.find(".com");
		auto orgFind = baseUrlNew.find(".org");
		if (httpsFind != std::string::npos && comFind != std::string::npos) {
			addressString =
				baseUrlNew.substr(httpsFind + std::string("https://").size(), comFind + std::string(".com").size() - std::string("https://").size());
		} else if (httpsFind != std::string::npos && orgFind != std::string::npos) {
			addressString =
				baseUrlNew.substr(httpsFind + std::string("https://").size(), orgFind + std::string(".org").size() - std::string("https://").size());
		} else {
			addressString = baseUrlNew;
		}
		addrinfoWrapper hints{}, address{};
		hints->ai_family = AF_INET;
		hints->ai_socktype = SOCK_STREAM;
		hints->ai_protocol = IPPROTO_TCP;

		if (getaddrinfo(addressString.c_str(), std::to_string(portNew).c_str(), hints, address)) {
			socket = INVALID_SOCKET;
			throw DiscordCoreAPI::DCAException{ reportError("TCPConnection::connect::getaddrinfo(), to: " + baseUrlNew) };
		}

		if (socket = ::socket(address->ai_family, address->ai_socktype, address->ai_protocol); socket == INVALID_SOCKET) {
			socket = INVALID_SOCKET;
			throw DiscordCoreAPI::DCAException{ reportError("TCPConnection::connect::socket(), to: " + baseUrlNew) };
		}

		bool boolOptionVal{ true };
		if (setsockopt(socket, SOL_SOCKET, SO_KEEPALIVE, reinterpret_cast<const char*>(&boolOptionVal), sizeof(int32_t))) {
			socket = INVALID_SOCKET;
			throw DiscordCoreAPI::DCAException{ reportError("TCPConnection::connect::setsockopt(), to: " + baseUrlNew) };
		}

		if (::connect(socket, address->ai_addr, static_cast<int32_t>(address->ai_addrlen)) == SOCKET_ERROR) {
			socket = INVALID_SOCKET;
			throw DiscordCoreAPI::DCAException{ reportError("TCPConnection::connect::connect(), to: " + baseUrlNew) };
		}

		std::unique_lock lock{ SSLConnectionInterface::mutex };
		if (ssl = SSL_new(context); !ssl) {
			socket = INVALID_SOCKET;
			ssl = nullptr;
			throw DiscordCoreAPI::DCAException{ reportSSLError("TCPConnection::connect::SSL_new(), to: " + baseUrlNew) };
		}
		lock.unlock();

		if (auto result{ SSL_set_fd(ssl, socket) }; result != 1) {
			socket = INVALID_SOCKET;
			ssl = nullptr;
			throw DiscordCoreAPI::DCAException{ reportSSLError("TCPConnection::connect::SSL_set_fd(), to: " + baseUrlNew, result, ssl) };
		}

		/* SNI */
		if (auto result{ SSL_set_tlsext_host_name(ssl, addressString.c_str()) }; result != 1) {
			socket = INVALID_SOCKET;
			ssl = nullptr;
			throw DiscordCoreAPI::DCAException{ reportSSLError("TCPConnection::connect::SSL_set_tlsext_host_name(), to: " + baseUrlNew, result,
				ssl) };
		}

		if (auto result{ SSL_connect(ssl) }; result != 1) {
			socket = INVALID_SOCKET;
			ssl = nullptr;
			throw DiscordCoreAPI::DCAException{ reportSSLError("TCPConnection::connect::SSL_connect(), to: " + baseUrlNew, result, ssl) };
		}

#ifdef _WIN32
		u_long value02{ 1 };
		if (auto returnData{ ioctlsocket(socket, FIONBIO, &value02) }; returnData == SOCKET_ERROR) {
			socket = INVALID_SOCKET;
			ssl = nullptr;
			throw DiscordCoreAPI::DCAException{ reportError("TCPConnection::connect::ioctlsocket(), to: " + baseUrlNew) };
		}
#else
		if (auto returnData{ fcntl(socket, F_SETFL, fcntl(socket, F_GETFL, 0) | O_NONBLOCK) }; returnData == SOCKET_ERROR) {
			socket = INVALID_SOCKET;
			ssl = nullptr;
			throw DiscordCoreAPI::DCAException{ reportError("TCPConnection::connect::ioctlsocket(), to: " + baseUrlNew) };
		}
#endif
	}

	void TCPConnection::writeData(std::string_view dataToWrite, bool priority) {
		if (areWeStillConnected()) {
			if (dataToWrite.size() > 0 && ssl) {
				if (priority && dataToWrite.size() < maxBufferSize) {
					outputBuffer.clear();
					outputBuffer.writeData(
						std::basic_string_view<uint8_t>{ reinterpret_cast<const uint8_t*>(dataToWrite.data()), dataToWrite.size() });
					if (!processWriteData()) {
						return;
					}
				} else {
					uint64_t remainingBytes{ dataToWrite.size() };
					while (remainingBytes > 0) {
						uint64_t amountToCollect{};
						dataToWrite.size() >= maxBufferSize ? amountToCollect = maxBufferSize : amountToCollect = dataToWrite.size();
						outputBuffer.writeData(
							std::basic_string_view<uint8_t>{ reinterpret_cast<const uint8_t*>(dataToWrite.data()), amountToCollect });
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

	void TCPConnection::processIO(int32_t waitTimeInMs) {
		if (socket == INVALID_SOCKET) {
			return;
		};
		pollfd readWriteSet{ .fd = static_cast<SOCKET>(socket) };
		if ((outputBuffer.getUsedSpace() > 0 || writeWantWrite || readWantWrite) && !readWantRead && !writeWantRead) {
			readWriteSet.events = POLLIN | POLLOUT;
		} else if (!writeWantWrite && !readWantWrite) {
			readWriteSet.events = POLLIN;
		}
		if (auto returnValue = poll(&readWriteSet, 1, waitTimeInMs); returnValue == SOCKET_ERROR) {
			socket = INVALID_SOCKET;
			ssl = nullptr;
			throw ConnectionError{ reportSSLError("TCPSSLClient::processIO() 01") + reportError("TCPSSLClient::processIO() 01"),
				ConnectionErrorType::SOCKET_Error };
		} else if (returnValue == 0) {
			return;
		} else {
			if (readWriteSet.revents & POLLIN || (POLLOUT && readWantWrite)) {
				if (!processReadData()) {
					socket = INVALID_SOCKET;
					ssl = nullptr;
					throw ConnectionError{ reportSSLError("TCPSSLClient::processIO() 02") + reportError("TCPSSLClient::processIO() 02"),
						ConnectionErrorType::READ_Error };
				}
			}
			if (readWriteSet.revents & POLLOUT || (POLLIN && writeWantRead)) {
				if (!processWriteData()) {
					socket = INVALID_SOCKET;
					ssl = nullptr;
					throw ConnectionError{ reportSSLError("TCPSSLClient::processIO() 03") + reportError("TCPSSLClient::processIO() 03"),
						ConnectionErrorType::WRITE_Error };
				}
			}
			if (readWriteSet.revents & POLLERR) {
				socket = INVALID_SOCKET;
				ssl = nullptr;
				throw ConnectionError{ reportSSLError("TCPSSLClient::processIO() 04") + reportError("TCPSSLClient::processIO() 04"),
					ConnectionErrorType::POLLERR_Error };
			}
			if (readWriteSet.revents & POLLNVAL) {
				socket = INVALID_SOCKET;
				ssl = nullptr;
				throw ConnectionError{ reportSSLError("TCPSSLClient::processIO() 05") + reportError("TCPSSLClient::processIO() 05"),
					ConnectionErrorType::POLLNVAL_Error };
			}
			if (readWriteSet.revents & POLLHUP) {
				socket = INVALID_SOCKET;
				ssl = nullptr;
			}
		}
		return;
	}

	std::basic_string_view<uint8_t> TCPConnection::getInputBuffer() noexcept {
		return inputBuffer.readData();
	}

	bool TCPConnection::areWeStillConnected() noexcept {
		if (socket.operator SOCKET() != INVALID_SOCKET) {
			return true;
		} else {
			return false;
		}
	}

	bool TCPConnection::processWriteData() {
		writeWantRead = false;
		writeWantWrite = false;
		if (outputBuffer.getUsedSpace() > 0 && this->socket != INVALID_SOCKET) {
			uint64_t bytesToWrite{ outputBuffer.getCurrentTail()->getUsedSpace() };

			uint64_t writtenBytes{};
			auto returnData{ SSL_write_ex(ssl, outputBuffer.readData().data(), bytesToWrite, &writtenBytes) };
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
					socket = INVALID_SOCKET;
					ssl = nullptr;
					return false;
				}
			}
		}
		return true;
	}

	bool TCPConnection::processReadData() {
		readWantRead = false;
		readWantWrite = false;
		if (!inputBuffer.isItFull() && this->socket != INVALID_SOCKET) {
			do {
				uint64_t readBytes{};
				uint64_t bytesToRead{ maxBufferSize };
				auto returnData{ SSL_read_ex(ssl, inputBuffer.getCurrentHead()->getCurrentHead(), bytesToRead, &readBytes) };
				auto errorValue{ SSL_get_error(ssl, returnData) };
				if (static_cast<int64_t>(readBytes) > 0) {
					inputBuffer.getCurrentHead()->modifyReadOrWritePosition(RingBufferAccessType::Write, readBytes);
					inputBuffer.modifyReadOrWritePosition(RingBufferAccessType::Write, 1);
					bytesRead += readBytes;
					if (!areWeAStandaloneSocket) {
						handleBuffer();
					}
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
						return false;
					}
					default: {
						socket = INVALID_SOCKET;
						ssl = nullptr;
						return false;
					}
				}
			} while (areWeStillConnected() && SSL_pending(ssl) && !inputBuffer.isItFull() && !readWantRead);
		}
		return true;
	}

	int64_t TCPConnection::getBytesRead() noexcept {
		return bytesRead;
	}

	void TCPConnection::reset() noexcept {
		outputBuffer.clear();
		inputBuffer.clear();
		bytesRead = 0;
	}

	SSL_CTXWrapper SSLConnectionInterface::context{};
	std::mutex SSLConnectionInterface::mutex{};
}