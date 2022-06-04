/*
*
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
/// SSLClents.cpp - Source file for the "SSL Client" stuff.
/// Dec 12, 2021
/// https://discordcoreapi.com
/// \file SSLClients.cpp

#include <discordcoreapi/SSLClients.hpp>

namespace DiscordCoreInternal {

	std::string reportSSLError(const std::string& errorPosition, int32_t errorValue = 0, SSL* ssl = nullptr) noexcept {
		std::stringstream theStream{};
		if (ssl) {
			theStream << DiscordCoreAPI::shiftToBrightRed() << errorPosition << SSL_get_error(ssl, errorValue) << DiscordCoreAPI::reset() << std::endl;
		} else {
			theStream << DiscordCoreAPI::shiftToBrightRed() << errorPosition << DiscordCoreAPI::reset() << std::endl;
		}
		ERR_print_errors_fp(stdout);
		return theStream.str();
	}

	std::string reportError(const std::string& errorPosition, int32_t errorValue) noexcept {
		std::stringstream theStream{};
		theStream << DiscordCoreAPI::shiftToBrightRed() << errorPosition << errorValue << ", ";
#ifdef _WIN32
		std::unique_ptr<char[]> string{ std::make_unique<char[]>(1024) };
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), string.get(), 1024, NULL);
		theStream << WSAGetLastError() << ", " << string << DiscordCoreAPI::reset() << std::endl;
#else
		theStream << strerror(errno) << DiscordCoreAPI::reset() << std::endl;
#endif
		return theStream.str();
	}

	void HttpSSLClient::connect(const std::string& baseUrl, const std::string& portNew) {
		std::string stringNew{};
		if (baseUrl.find(".com") != std::string::npos) {
			stringNew =
				baseUrl.substr(baseUrl.find("https://") + std::string("https://").size(), baseUrl.find(".com") + std::string(".com").size() - std::string("https://").size());
		} else if (baseUrl.find(".org") != std::string::npos) {
			stringNew =
				baseUrl.substr(baseUrl.find("https://") + std::string("https://").size(), baseUrl.find(".org") + std::string(".org").size() - std::string("https://").size());
		}

		addrinfoWrapper hints{ nullptr }, address{ nullptr };
		hints->ai_family = AF_INET;
		hints->ai_socktype = SOCK_STREAM;
		hints->ai_protocol = IPPROTO_TCP;

		if (auto resultValue = getaddrinfo(stringNew.c_str(), portNew.c_str(), hints, address); resultValue == SOCKET_ERROR) {
			ConnectionError theError{ reportError("getaddrinfo() Error: ", resultValue) };
			throw theError;
		}

		if (this->theSocket = socket(address->ai_family, address->ai_socktype, address->ai_protocol); this->theSocket == SOCKET_ERROR) {
			ConnectionError theError{ reportError("socket() Error: ", this->theSocket) };
			throw theError;
		}

		int32_t value{ this->maxBufferSize + 1 };
		if (auto resultValue = setsockopt(this->theSocket, SOL_SOCKET, SO_SNDBUF, static_cast<char*>(static_cast<void*>(&value)), sizeof(value)); resultValue == SOCKET_ERROR) {
			ConnectionError theError{ reportError("setsockopt() Error: ", resultValue) };
			throw theError;
		}

#ifdef _WIN32
		char optionValue{ true };
		if (auto resultValue = setsockopt(this->theSocket, IPPROTO_TCP, TCP_NODELAY, &optionValue, sizeof(optionValue)); resultValue == SOCKET_ERROR) {
			ConnectionError theError{ reportError("setsockopt() Error: ", resultValue) };
			throw theError;
		}
#else
		int32_t optionValue{ 1 };
		if (auto resultValue = setsockopt(this->theSocket, SOL_TCP, TCP_NODELAY, &optionValue, sizeof(optionValue)); resultValue == SOCKET_ERROR) {
			ConnectionError theError{ reportError("setsockopt() Error: ", resultValue) };
			throw theError;
		}
#endif

		if (auto resultValue = ::connect(this->theSocket, address->ai_addr, static_cast<int32_t>(address->ai_addrlen)); resultValue == SOCKET_ERROR) {
			ConnectionError theError{ reportError("connect() Error: ", resultValue) };
			throw theError;
		}

		if (this->context = SSL_CTX_new(TLS_client_method()); this->context == nullptr) {
			ConnectionError theError{ reportSSLError("SSL_CTX_new() Error: ") };
			throw theError;
		}

		auto options{ SSL_CTX_get_options(this->context) };
		if (SSL_CTX_set_options(this->context, SSL_OP_IGNORE_UNEXPECTED_EOF) != (options | SSL_OP_IGNORE_UNEXPECTED_EOF)) {
			ConnectionError theError{ reportSSLError("SSL_CTX_set_options() Error: ") };
			throw theError;
		}

		if (this->ssl = SSL_new(this->context); this->ssl == nullptr) {
			ConnectionError theError{ reportSSLError("SSL_new() Error: ") };
			throw theError;
		}

		if (auto resultValue = SSL_set_fd(this->ssl, this->theSocket); !resultValue) {
			ConnectionError theError{ reportSSLError("SSL_set_fd() Error: ", resultValue, this->ssl) };
			throw theError;
		}

		if (auto resultValue = SSL_set_tlsext_host_name(this->ssl, stringNew.c_str()); !resultValue) {
			ConnectionError theError{ reportSSLError("SSL_set_tlsext_host_name() Error: ", resultValue, this->ssl) };
			throw theError;
		}

		if (auto resultValue = SSL_connect(this->ssl); !resultValue) {
			ConnectionError theError{ reportSSLError("SSL_connect() Error: ", resultValue, this->ssl) };
			throw theError;
		}
	}

	void HttpSSLClient::writeData(const std::string& data) noexcept {
		this->outputBuffer.insert(this->outputBuffer.end(), data.begin(), data.end());
	}

	std::string& HttpSSLClient::getInputBuffer() noexcept {
		return this->inputBuffer;
	}

	void HttpSSLClient::processIO() {
#ifdef _WIN32
		fd_set writeSet{}, readSet{};
		int32_t nfds{ 0 };
		FD_ZERO(&writeSet);
		FD_ZERO(&readSet);

		if (this->outputBuffer.size() > 0 && !this->wantRead) {
			FD_SET(this->theSocket, &writeSet);
			nfds = this->theSocket > nfds ? this->theSocket : nfds;
		}
		FD_SET(this->theSocket, &readSet);
		nfds = this->theSocket > nfds ? this->theSocket : nfds;

		timeval checkTime{ .tv_usec = 10000 };
		if (auto resultValue = select(nfds + 1, &readSet, &writeSet, nullptr, &checkTime); resultValue == SOCKET_ERROR) {
			ProcessingError theError{ reportError("select() Error: ", resultValue) };
			throw theError;
		} else if (resultValue == 0) {
			return;
		}
#else
		epoll_event writeEvent{}, readEvent{}, events[1]{};
		epollWrapper epoll{ nullptr };
		bool writing{ false };

		if (this->outputBuffer.size() > 0 && !this->wantRead) {
			writing = true;
			writeEvent.events = EPOLLOUT;
			writeEvent.data.fd = this->theSocket;
			if (auto resultValue = epoll_ctl(epoll, EPOLL_CTL_ADD, this->theSocket, &writeEvent); resultValue == SOCKET_ERROR) {
				ProcessingError theError{ reportError("epoll_ctl() Error: ", resultValue) };
				throw theError;
			}
		} else {
			readEvent.events = EPOLLIN;
			readEvent.data.fd = this->theSocket;
			if (auto resultValue = epoll_ctl(epoll, EPOLL_CTL_ADD, this->theSocket, &readEvent); resultValue == SOCKET_ERROR) {
				ProcessingError theError{ reportError("epoll_ctl() Error: ", resultValue) };
				throw theError;
			}
		}

		if (auto resultValue = epoll_wait(epoll, events, 1, 1); resultValue == SOCKET_ERROR) {
			ProcessingError theError{ reportError("epoll_wait() Error: ", resultValue) };
			throw theError;
		} else if (resultValue == 0) {
			return;
		}

#endif

#ifdef _WIN32
		if (FD_ISSET(this->theSocket, &readSet)) {
#else
		if (!writing) {
#endif
			this->wantRead = false;
			std::string serverToClientBuffer{};
			serverToClientBuffer.resize(this->maxBufferSize);
			size_t readBytes{ 0 };
			auto resultValue{ SSL_read_ex(this->ssl, serverToClientBuffer.data(), this->maxBufferSize, &readBytes) };
			auto errorValue{ SSL_get_error(this->ssl, resultValue) };
			switch (errorValue) {
				case SSL_ERROR_NONE: {
					if (readBytes > 0) {
						this->inputBuffer.insert(this->inputBuffer.end(), serverToClientBuffer.begin(), serverToClientBuffer.begin() + readBytes);
					}
					return;
				}
				case SSL_ERROR_SYSCALL: {
					[[fallthrough]];
				}
				case SSL_ERROR_ZERO_RETURN: {
					std::string theResultString{ reportSSLError("HttpSSLClient::processIO::SSL_read_ex() Error: ", resultValue, this->ssl) + "\n" +
						reportError("HttpSSLClient::processIO::SSL_read_ex() Error: ", resultValue) };
					throw ProcessingError{ theResultString };
				}
				case SSL_ERROR_WANT_READ: {
					this->wantRead = true;
					[[fallthrough]];
				}
				case SSL_ERROR_WANT_WRITE: {
					return;
				}
				default: {
					std::string theResultString{ reportSSLError("HttpSSLClient::processIO::SSL_read_ex() Error: ", resultValue, this->ssl) + "\n" +
						reportError("HttpSSLClient::processIO::SSL_read_ex() Error: ", resultValue) };
					throw ProcessingError{ theResultString };
				}
			}
		}
#ifdef _WIN32
		else if (FD_ISSET(this->theSocket, &writeSet)) {
#else
		else {
#endif
			size_t writtenBytes{ 0 };
			auto resultValue{ SSL_write_ex(this->ssl, this->outputBuffer.data(), this->outputBuffer.size(), &writtenBytes) };
			auto errorValue{ SSL_get_error(this->ssl, resultValue) };
			switch (errorValue) {
				case SSL_ERROR_NONE: {
					this->outputBuffer.clear();
					return;
				}
				case SSL_ERROR_SYSCALL: {
					[[fallthrough]];
				}
				case SSL_ERROR_ZERO_RETURN: {
					std::string theResultString{ reportSSLError("HttpSSLClient::processIO::SSL_write_ex() Error: ", resultValue, this->ssl) + "\n" +
						reportError("HttpSSLClient::processIO::SSL_write_ex() Error: ", resultValue) };
					throw ProcessingError{ theResultString };
				}
				case SSL_ERROR_WANT_READ: {
					this->wantRead = true;
					[[fallthrough]];
				}
				case SSL_ERROR_WANT_WRITE: {
					return;
				}
				default: {
					std::string theResultString{ reportSSLError("HttpSSLClient::processIO::SSL_write_ex() Error: ", resultValue, this->ssl) + "\n" +
						reportError("HttpSSLClient::processIO::SSL_write_ex() Error: ", resultValue) };
					throw ProcessingError{ theResultString };
				}
			}
		}
	}

	WebSocketSSLClient::WebSocketSSLClient(int32_t maxBufferSizeNew) noexcept {
		this->maxBufferSize = maxBufferSizeNew;
	};

	void WebSocketSSLClient::connect(const std::string& baseUrlNew, const std::string& portNew) {
		addrinfoWrapper hints{ nullptr }, address{ nullptr };
		hints->ai_family = AF_INET;
		hints->ai_socktype = SOCK_STREAM;
		hints->ai_protocol = IPPROTO_TCP;

		if (auto resultValue = getaddrinfo(baseUrlNew.c_str(), portNew.c_str(), hints, address); resultValue == SOCKET_ERROR) {
			ConnectionError theError{ reportError("getaddrinfo() Error: ", this->theSocket) };
			throw theError;
		}

		if (this->theSocket = socket(address->ai_family, address->ai_socktype, address->ai_protocol); this->theSocket == SOCKET_ERROR) {
			ConnectionError theError{ reportError("socket() Error: ", this->theSocket) };
			throw theError;
		}

		int32_t value{ this->maxBufferSize + 1 };
		if (auto resultValue = setsockopt(this->theSocket, SOL_SOCKET, SO_SNDBUF, static_cast<char*>(static_cast<void*>(&value)), sizeof(value)); resultValue == SOCKET_ERROR) {
			ConnectionError theError{ reportError("setsockopt() Error: ", this->theSocket) };
			throw theError;
		}

#ifdef _WIN32
		char optionValue{ true };
		if (auto resultValue = setsockopt(this->theSocket, IPPROTO_TCP, TCP_NODELAY, &optionValue, sizeof(optionValue)); resultValue == SOCKET_ERROR) {
			ConnectionError theError{ reportError("setsockopt() Error: ", this->theSocket) };
			throw theError;
		}
#else
		int32_t optionValue{ 1 };
		if (auto resultValue = setsockopt(this->theSocket, SOL_TCP, TCP_NODELAY, &optionValue, sizeof(optionValue)); resultValue == SOCKET_ERROR) {
			ConnectionError theError{ reportError("setsockopt() Error: ", this->theSocket) };
			throw theError;
		}
#endif

		if (auto resultValue = ::connect(this->theSocket, address->ai_addr, static_cast<int32_t>(address->ai_addrlen)); resultValue == SOCKET_ERROR) {
			ConnectionError theError{ reportError("setsockopt() Error: ", this->theSocket) };
			throw theError;
		}

		if (this->context = SSL_CTX_new(TLS_client_method()); this->context == nullptr) {
			ConnectionError theError{ reportSSLError("SSL_CTX_new() Error: ") };
			throw theError;
		}

		auto options{ SSL_CTX_get_options(this->context) };
		if (SSL_CTX_set_options(this->context, SSL_OP_IGNORE_UNEXPECTED_EOF) != (options | SSL_OP_IGNORE_UNEXPECTED_EOF)) {
			ConnectionError theError{ reportSSLError("SSL_CTX_set_options() Error: ") };
			throw theError;
		}

		if (this->ssl = SSL_new(this->context); this->ssl == nullptr) {
			ConnectionError theError{ reportSSLError("SSL_new() Error: ") };
			throw theError;
		}

		if (auto resultValue = SSL_set_fd(this->ssl, this->theSocket); !resultValue) {
			ConnectionError theError{ reportSSLError("SSL_new() Error: ", resultValue, this->ssl) };
			throw theError;
		}

		/* SNI */
		if (auto resultValue = SSL_set_tlsext_host_name(this->ssl, baseUrlNew.c_str()); !resultValue) {
			ConnectionError theError{ reportSSLError("SSL_set_tlsext_host_name() Error: ", resultValue, this->ssl) };
			throw theError;
		}

		if (auto resultValue = SSL_connect(this->ssl); !resultValue) {
			ConnectionError theError{ reportSSLError("SSL_connect() Error: ", resultValue, this->ssl) };
			throw theError;
		}
		this->areWeConnected = true;
	}

	void WebSocketSSLClient::processIO(int32_t waitTimeInMicroSeconds) {
		if (this->areWeConnected) {
#ifdef _WIN32
			fd_set writeSet{}, readSet{};
			int32_t nfds{ 0 };
			FD_ZERO(&writeSet);
			FD_ZERO(&readSet);

			if (this->outputBuffer.size() > 0 && !this->wantRead) {
				FD_SET(this->theSocket, &writeSet);
				nfds = this->theSocket > nfds ? this->theSocket : nfds;
			}
			FD_SET(this->theSocket, &readSet);
			nfds = this->theSocket > nfds ? this->theSocket : nfds;

			timeval checkTime{ .tv_usec = waitTimeInMicroSeconds };
			if (auto resultValue = select(nfds + 1, &readSet, &writeSet, nullptr, &checkTime); resultValue == SOCKET_ERROR) {
				this->areWeConnected = false;
				ProcessingError theError{ reportError("select() Error: ", resultValue) };
				throw theError;
			} else if (resultValue == 0) {
				return;
			}
#else
			epoll_event writeEvent{}, readEvent{}, events[1]{};
			epollWrapper epoll{ nullptr };
			bool writing{ false };

			if (this->outputBuffer.size() > 0 && !this->wantRead) {
				writing = true;
				writeEvent.events = EPOLLOUT;
				writeEvent.data.fd = this->theSocket;
				if (auto resultValue = epoll_ctl(epoll, EPOLL_CTL_ADD, this->theSocket, &writeEvent); resultValue == SOCKET_ERROR) {
					this->areWeConnected = false;
					ProcessingError theError{ reportError("epoll_ctl() Error: ", resultValue) };
					throw theError;
				}
			} else {
				readEvent.events = EPOLLIN;
				readEvent.data.fd = this->theSocket;
				if (auto resultValue = epoll_ctl(epoll, EPOLL_CTL_ADD, this->theSocket, &readEvent); resultValue == SOCKET_ERROR) {
					this->areWeConnected = false;
					ProcessingError theError{ reportError("epoll_ctl() Error: ", resultValue) };
					throw theError;
				}
			}

			if (auto resultValue = epoll_wait(epoll, events, 1, waitTimeInMicroSeconds / 1000); resultValue == SOCKET_ERROR) {
				this->areWeConnected = false;
				ProcessingError theError{ reportError("epoll_wait() Error: ", resultValue) };
				throw theError;
			} else if (resultValue == 0) {
				return;
			}

#endif

#ifdef _WIN32
			if (FD_ISSET(this->theSocket, &readSet)) {
#else
			if (!writing) {
#endif
				this->wantRead = false;
				std::string serverToClientBuffer{};
				serverToClientBuffer.resize(this->maxBufferSize);
				size_t readBytes{ 0 };
				auto resultValue{ SSL_read_ex(this->ssl, serverToClientBuffer.data(), this->maxBufferSize, &readBytes) };
				auto errorValue{ SSL_get_error(this->ssl, resultValue) };
				switch (errorValue) {
					case SSL_ERROR_NONE: {
						if (readBytes > 0) {
							this->inputBuffer.insert(this->inputBuffer.end(), serverToClientBuffer.begin(), serverToClientBuffer.begin() + readBytes);
						}
						return;
					}
					case SSL_ERROR_SYSCALL: {
						[[fallthrough]];
					}
					case SSL_ERROR_ZERO_RETURN: {
						this->areWeConnected = false;
						std::string theResultString{ reportSSLError("WebSocketSSLClient::processIO::SSL_read_ex() Error: ", resultValue, this->ssl) + "\n" +
							reportError("WebSocketSSLClient::processIO::SSL_read_ex() Error: ", resultValue) };
						throw ProcessingError{ theResultString };
					}
					case SSL_ERROR_WANT_READ: {
						this->wantRead = true;
						[[fallthrough]];
					}
					case SSL_ERROR_WANT_WRITE: {
						return;
					}
					default: {
						this->areWeConnected = false;
						std::string theResultString{ reportSSLError("WebSocketSSLClient::processIO::SSL_read_ex() Error: ", resultValue, this->ssl) + "\n" +
							reportError("WebSocketSSLClient::processIO::SSL_read_ex() Error: ", resultValue) };
						throw ProcessingError{ theResultString };
					}
				}
			}
#ifdef _WIN32
			else if (FD_ISSET(this->theSocket, &writeSet)) {
#else
			else {
#endif
				this->wantRead = false;
				size_t writtenBytes{ 0 };
				auto resultValue{ SSL_write_ex(this->ssl, this->outputBuffer.data(), this->outputBuffer.size(), &writtenBytes) };
				auto errorValue{ SSL_get_error(this->ssl, resultValue) };
				switch (errorValue) {
					case SSL_ERROR_NONE: {
						this->outputBuffer.clear();
						return;
					}
					case SSL_ERROR_SYSCALL: {
						[[fallthrough]];
					}
					case SSL_ERROR_ZERO_RETURN: {
						this->areWeConnected = false;
						std::string theResultString{ reportSSLError("WebSocketSSLClient::processIO::SSL_write_ex() Error: ", resultValue, this->ssl) + "\n" +
							reportError("WebSocketSSLClient::processIO::SSL_write_ex() Error: ", resultValue) };
						throw ProcessingError{ theResultString };
					}
					case SSL_ERROR_WANT_READ: {
						this->wantRead = true;
						[[fallthrough]];
					}
					case SSL_ERROR_WANT_WRITE: {
						return;
					}
					default: {
						this->areWeConnected = false;
						std::string theResultString{ reportSSLError("WebSocketSSLClient::processIO::SSL_write_ex() Error: ", resultValue, this->ssl) + "\n" +
							reportError("WebSocketSSLClient::processIO::SSL_write_ex() Error: ", resultValue) };
						throw ProcessingError{ theResultString };
					}
				}
			}
		}
	}

	void WebSocketSSLClient::writeData(const std::string& data) noexcept {
		this->outputBuffer.insert(this->outputBuffer.end(), data.begin(), data.end());
	}

	std::string WebSocketSSLClient::getInputBuffer() noexcept {
		std::string returnString = this->inputBuffer;
		this->inputBuffer.clear();
		return returnString;
	}

	int64_t WebSocketSSLClient::getBytesRead() noexcept {
		return this->bytesRead;
	}

	bool WebSocketSSLClient::didWeConnect() noexcept {
		return this->areWeConnected;
	}

	void DatagramSocketSSLClient::connect(const std::string& baseUrlNew, const std::string& portNew) {
		addrinfoWrapper hints{ nullptr }, address{ nullptr };
		hints->ai_family = AF_INET;
		hints->ai_socktype = SOCK_DGRAM;
		hints->ai_protocol = IPPROTO_UDP;

		if (auto resultValue = getaddrinfo(baseUrlNew.c_str(), portNew.c_str(), hints, address); resultValue == SOCKET_ERROR) {
			ConnectionError theError{ reportError("getaddrinfo() Error: ", this->theSocket) };
			throw theError;
		}

		if (this->theSocket = socket(address->ai_family, address->ai_socktype, address->ai_protocol); this->theSocket == SOCKET_ERROR) {
			ConnectionError theError{ reportError("socket() Error: ", this->theSocket) };
			throw theError;
		}

		if (auto resultValue = ::connect(this->theSocket, address->ai_addr, static_cast<int32_t>(address->ai_addrlen)); resultValue == SOCKET_ERROR) {
			ConnectionError theError{ reportError("connect() Error: ", this->theSocket) };
			throw theError;
		}

#ifdef _WIN32
		u_long value{ 1 };
		if (auto resultValue = ioctlsocket(this->theSocket, FIONBIO, &value); resultValue == SOCKET_ERROR) {
			ConnectionError theError{ reportError("ioctlsocket() Error: ", this->theSocket) };
			throw theError;
		}
#else
		if (auto resultValue = fcntl(this->theSocket, F_SETFL, fcntl(this->theSocket, F_GETFL, 0) | O_NONBLOCK); resultValue == SOCKET_ERROR) {
			ConnectionError theError{ reportError("fcntl() Error: ", this->theSocket) };
			throw theError;
		}
#endif
		if (this->datagramBio = BIO_new_dgram(this->theSocket, BIO_CLOSE); this->datagramBio == nullptr) {
			ConnectionError theError{ reportSSLError("BIO_new_dgram() Error: ") };
			throw theError;
		}

		if (auto resultValue = BIO_ctrl(this->datagramBio, BIO_CTRL_DGRAM_SET_CONNECTED, 0, &address); resultValue == 0) {
			ConnectionError theError{ reportSSLError("BIO_ctrl() Error: ") };
			throw theError;
		}
	}

	void DatagramSocketSSLClient::writeData(const std::string& data) {
		size_t writtenBytes{ 0 };
		if (auto resultValue = BIO_write_ex(this->datagramBio, data.data(), data.size(), &writtenBytes); !resultValue) {
			std::string theResultString{ reportSSLError("DatagramSocketSSLClient::writeData::BIO_write_ex() Error: ", resultValue) + "\n" +
				reportError("DatagramSocketSSLClient::writeData::BIO_write_ex() Error: ", resultValue) };
			throw ProcessingError{ theResultString };
		}
		return;
	}

	std::string& DatagramSocketSSLClient::getInputBuffer() noexcept {
		return this->inputBuffer;
	}

	bool DatagramSocketSSLClient::readData(bool doWeClear) noexcept {
		std::string serverToClientBuffer{};
		serverToClientBuffer.resize(this->maxBufferSize);
		size_t readBytes{ 0 };
		if (auto resultValue = BIO_read_ex(this->datagramBio, serverToClientBuffer.data(), this->maxBufferSize, &readBytes); resultValue == 1) {
			if (readBytes > 0) {
				this->inputBuffer.insert(this->inputBuffer.end(), serverToClientBuffer.begin(), serverToClientBuffer.begin() + readBytes);
				if (doWeClear) {
					this->inputBuffer.clear();
				}
			}
			return true;
		}
		return true;
	}

	int64_t DatagramSocketSSLClient::getBytesRead() noexcept {
		return this->bytesRead;
	}

	std::string HttpSSLClient::soundcloudCertPathStatic{};
	std::string HttpSSLClient::defaultCertPathStatic{};
	std::string HttpSSLClient::googleCertPathStatic{};
	std::mutex HttpSSLClient::theMutex{};
}
