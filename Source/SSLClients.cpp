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

	void reportSSLError(const std::string& errorPosition, int32_t errorValue = 0, SSL* ssl = nullptr) noexcept {
		if (ssl) {
			std::cout << DiscordCoreAPI::shiftToBrightRed() << errorPosition << SSL_get_error(ssl, errorValue) << std::endl;
		} else {
			std::cout << DiscordCoreAPI::shiftToBrightRed() << errorPosition << std::endl;
		}
		ERR_print_errors_fp(stdout);
		std::cout << std::endl << DiscordCoreAPI::reset();
	}

	void reportError(const std::string& errorPosition, int32_t errorValue) noexcept {
		std::cout << DiscordCoreAPI::shiftToBrightRed() << errorPosition << errorValue << ", ";
#ifdef _WIN32
		char* s = NULL;
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, WSAGetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), static_cast<LPSTR>(static_cast<void*>(&s)), 0, NULL);
		std::cout << WSAGetLastError() << ", " << s << std::endl << DiscordCoreAPI::reset();
#else
		std::cout << strerror(errno) << std::endl << DiscordCoreAPI::reset();
#endif
	}

	bool HttpSSLClient::connect(const std::string& baseUrl, bool doWePrintErrorNew, const std::string& portNew) noexcept {
		this->doWePrintError = doWePrintErrorNew;
		std::string stringNew{};
		if (baseUrl.find(".com") != std::string::npos) {
			stringNew =
				baseUrl.substr(baseUrl.find("https://") + std::string("https://").size(), baseUrl.find(".com") + std::string(".com").size() - std::string("https://").size());
		} else if (baseUrl.find(".org") != std::string::npos) {
			stringNew =
				baseUrl.substr(baseUrl.find("https://") + std::string("https://").size(), baseUrl.find(".org") + std::string(".org").size() - std::string("https://").size());
		}

		addrinfoWrapper hints{ nullptr }, addrs{ nullptr };
		hints->ai_family = AF_UNSPEC;
		hints->ai_socktype = SOCK_STREAM;
		hints->ai_protocol = IPPROTO_TCP;

		if (auto returnValue = getaddrinfo(stringNew.c_str(), portNew.c_str(), hints, addrs); returnValue == SOCKET_ERROR) {
			if (this->doWePrintError) {
				reportError("getaddrinfo() Error: ", returnValue);
			}
			return false;
		}

		if (this->theSocket = socket(addrs->ai_family, addrs->ai_socktype, addrs->ai_protocol); this->theSocket == SOCKET_ERROR) {
			if (this->doWePrintError) {
				reportError("socket() Error: ", this->theSocket);
			}
			return false;
		}

		if (auto returnValue = ::connect(this->theSocket, addrs->ai_addr, static_cast<int32_t>(addrs->ai_addrlen)); returnValue == SOCKET_ERROR) {
			if (this->doWePrintError) {
				reportError("connect() Error: ", returnValue);
			}
			return false;
		}

#ifdef _WIN32
		char optionValue{ true };
		if (auto returnValue = setsockopt(this->theSocket, IPPROTO_TCP, TCP_NODELAY, &optionValue, sizeof(optionValue)); returnValue == SOCKET_ERROR) {
			if (this->doWePrintError) {
				reportError("setsockopt() Error: ", returnValue);
			}
			return false;
		}
#else
		int32_t optionValue{ 1 };
		if (auto returnValue = setsockopt(this->theSocket, SOL_TCP, TCP_NODELAY, &optionValue, sizeof(optionValue)); returnValue == SOCKET_ERROR) {
			if (this->doWePrintError) {
				reportError("setsockopt() Error: ", returnValue);
			}
			return false;
		}
#endif

		if (this->context = SSL_CTX_new(TLS_client_method()); this->context == nullptr) {
			if (this->doWePrintError) {
				reportSSLError("SSL_CTX_new() Error: ");
			}
			return false;
		}

		auto options{ SSL_CTX_get_options(this->context) };
		if (SSL_CTX_set_options(this->context, SSL_OP_IGNORE_UNEXPECTED_EOF) != (options | SSL_OP_IGNORE_UNEXPECTED_EOF)) {
			if (this->doWePrintError) {
				reportSSLError("SSL_CTX_set_options() Error: ");
			}
			return false;
		}

		if (this->ssl = SSL_new(this->context); this->ssl == nullptr) {
			if (this->doWePrintError) {
				reportSSLError("SSL_new() Error: ");
			}
			return false;
		}

		if (auto returnValue = SSL_set_fd(this->ssl, this->theSocket); !returnValue) {
			if (this->doWePrintError) {
				reportSSLError("SSL_set_fd() Error: ", returnValue, this->ssl);
			}
			return false;
		}

		if (auto returnValue = SSL_set_tlsext_host_name(this->ssl, stringNew.c_str()); !returnValue) {
			if (this->doWePrintError) {
				reportSSLError("SSL_set_tlsext_host_name() Error: ", returnValue, this->ssl);
			}
			return false;
		}

		if (auto returnValue = SSL_connect(this->ssl); !returnValue) {
			if (this->doWePrintError) {
				reportSSLError("SSL_connect() Error: ", returnValue, this->ssl);
			}
			return false;
		}

		return true;
	}

	void HttpSSLClient::writeData(const std::string& data) noexcept {
		this->outputBuffer.insert(this->outputBuffer.end(), data.begin(), data.end());
	}

	std::string& HttpSSLClient::getInputBuffer() noexcept {
		return this->inputBuffer;
	}

	bool HttpSSLClient::processIO() noexcept {
#ifdef _WIN32
		fd_set writeSet{}, readSet{};
		int32_t nfds{ 0 };
		FD_ZERO(&writeSet);
		FD_ZERO(&readSet);

		if (this->outputBuffer.size() > 0 && !this->wantRead) {
			FD_SET(this->theSocket, &writeSet);
			nfds = this->theSocket > nfds ? this->theSocket : nfds;
		} else {
			FD_SET(this->theSocket, &readSet);
			nfds = this->theSocket > nfds ? this->theSocket : nfds;
		}

		timeval checkTime{ .tv_usec = 1000 };
		if (auto resultValue = select(nfds + 1, &readSet, &writeSet, nullptr, &checkTime); resultValue == SOCKET_ERROR) {
			if (this->doWePrintError) {
				reportError("select() Error: ", resultValue);
			}
			return false;
		} else if (resultValue == 0) {
			return true;
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
				if (this->doWePrintError) {
					reportError("epoll_ctl() Error: ", resultValue);
				}
				return false;
			}
		} else {
			readEvent.events = EPOLLIN;
			readEvent.data.fd = this->theSocket;
			if (auto resultValue = epoll_ctl(epoll, EPOLL_CTL_ADD, this->theSocket, &readEvent); resultValue == SOCKET_ERROR) {
				if (this->doWePrintError) {
					reportError("epoll_ctl() Error: ", resultValue);
				}
				return false;
			}
		}

		if (auto resultValue = epoll_wait(epoll, events, 1, 1); resultValue == SOCKET_ERROR) {
			if (this->doWePrintError) {
				reportError("epoll_wait() Error: ", resultValue);
			}
			return false;
		} else if (resultValue == 0) {
			return true;
		}

#endif

#ifdef _WIN32
		if (FD_ISSET(this->theSocket, &writeSet)) {
#else
		if (writing) {
#endif
			size_t writtenBytes{ 0 };
			auto returnValue{ SSL_write_ex(this->ssl, this->outputBuffer.data(), this->outputBuffer.size(), &writtenBytes) };
			auto errorValue{ SSL_get_error(this->ssl, returnValue) };
			switch (errorValue) {
				case SSL_ERROR_NONE: {
					this->outputBuffer.clear();
					return true;
				}
				case SSL_ERROR_SYSCALL: {
					if (this->doWePrintError) {
						reportSSLError("HttpSSLClient::processIO::SSL_write_ex() Error: ", returnValue, this->ssl);
						reportError("HttpSSLClient::processIO::SSL_write_ex() Error: ", returnValue);
					}
					[[fallthrough]];
				}
				case SSL_ERROR_ZERO_RETURN: {
					return false;
				}
				case SSL_ERROR_WANT_READ: {
					this->wantRead = true;
					[[fallthrough]];
				}
				case SSL_ERROR_WANT_WRITE: {
					return true;
				}
				default: {
					if (this->doWePrintError) {
						reportSSLError("HttpSSLClient::processIO::SSL_write_ex() Error: ", returnValue, this->ssl);
						reportError("HttpSSLClient::processIO::SSL_write_ex() Error: ", returnValue);
					}
					return false;
				}
			}
		}
#ifdef _WIN32
		else if (FD_ISSET(this->theSocket, &readSet)) {
#else
		else {
#endif
			this->wantRead = false;
			std::string serverToClientBuffer{};
			serverToClientBuffer.resize(this->maxBufferSize);
			size_t readBytes{ 0 };
			auto returnValue{ SSL_read_ex(this->ssl, serverToClientBuffer.data(), this->maxBufferSize, &readBytes) };
			auto errorValue{ SSL_get_error(this->ssl, returnValue) };
			switch (errorValue) {
				case SSL_ERROR_NONE: {
					if (readBytes > 0) {
						this->inputBuffer.insert(this->inputBuffer.end(), serverToClientBuffer.begin(), serverToClientBuffer.begin() + readBytes);
					}
					return true;
				}
				case SSL_ERROR_SYSCALL: {
					if (this->doWePrintError) {
						reportSSLError("HttpSSLClient::processIO::SSL_read_ex() Error: ", returnValue, this->ssl);
						reportError("HttpSSLClient::processIO::SSL_read_ex() Error: ", returnValue);
					}
					[[fallthrough]];
				}
				case SSL_ERROR_ZERO_RETURN: {
					return false;
				}
				case SSL_ERROR_WANT_READ: {
					this->wantRead = true;
					[[fallthrough]];
				}
				case SSL_ERROR_WANT_WRITE: {
					return true;
				}
				default: {
					if (this->doWePrintError) {
						reportSSLError("HttpSSLClient::processIO::SSL_read_ex() Error: ", returnValue, this->ssl);
						reportError("HttpSSLClient::processIO::SSL_read_ex() Error: ", returnValue);
					}
					return false;
				}
			}
		}
		return true;
	}

	WebSocketSSLClient::WebSocketSSLClient(const std::string& baseUrlNew, const std::string& portNew, bool doWePrintErrorNew, int64_t maxBufferSizeNew) noexcept
		: maxBufferSize(maxBufferSizeNew) {
		this->doWePrintError = doWePrintErrorNew;
		addrinfoWrapper resultAddress{ nullptr }, hints{ nullptr };
		hints->ai_family = AF_INET;
		hints->ai_socktype = SOCK_STREAM;
		hints->ai_protocol = IPPROTO_TCP;

		if (auto returnValue = getaddrinfo(baseUrlNew.c_str(), portNew.c_str(), hints, resultAddress); returnValue == SOCKET_ERROR) {
			if (this->doWePrintError) {
				reportError("getaddrinfo() Error: ", returnValue);
			}
			return;
		}

		if (this->theSocket = socket(resultAddress->ai_family, resultAddress->ai_socktype, resultAddress->ai_protocol); this->theSocket == SOCKET_ERROR) {
			if (this->doWePrintError) {
				reportError("socket() Error: ", this->theSocket);
			}
			return;
		}

		if (auto returnValue = connect(this->theSocket, resultAddress->ai_addr, static_cast<int32_t>(resultAddress->ai_addrlen)); returnValue == SOCKET_ERROR) {
			if (this->doWePrintError) {
				reportError("connect() Error: ", returnValue);
			}
			return;
		}

#ifdef _WIN32
		char optionValue{ true };
		if (auto returnValue = setsockopt(this->theSocket, IPPROTO_TCP, TCP_NODELAY, &optionValue, sizeof(optionValue)); returnValue == SOCKET_ERROR) {
			if (this->doWePrintError) {
				reportError("setsockopt() Error: ", returnValue);
			}
			return;
		}
#else
		int32_t optionValue{ 1 };
		if (auto returnValue = setsockopt(this->theSocket, SOL_TCP, TCP_NODELAY, &optionValue, sizeof(optionValue)); returnValue == SOCKET_ERROR) {
			if (this->doWePrintError) {
				reportError("setsockopt() Error: ", returnValue);
			}
			return;
		}
#endif

		if (this->context = SSL_CTX_new(TLS_client_method()); this->context == nullptr) {
			if (this->doWePrintError) {
				reportSSLError("SSL_CTX_new() Error: ");
			}
			return;
		}

		auto options{ SSL_CTX_get_options(this->context) };
		if (SSL_CTX_set_options(this->context, SSL_OP_IGNORE_UNEXPECTED_EOF) != (options | SSL_OP_IGNORE_UNEXPECTED_EOF)) {
			if (this->doWePrintError) {
				reportSSLError("SSL_CTX_set_options() Error: ");
			}
			return;
		}

		if (this->ssl = SSL_new(this->context); this->ssl == nullptr) {
			if (this->doWePrintError) {
				reportSSLError("SSL_new() Error: ");
			}
			return;
		}

		if (auto returnValue = SSL_set_fd(this->ssl, this->theSocket); !returnValue) {
			if (this->doWePrintError) {
				reportSSLError("SSL_set_fd() Error: ", returnValue, this->ssl);
			}
			return;
		}

		/* SNI */
		if (auto returnValue = SSL_set_tlsext_host_name(this->ssl, baseUrlNew.c_str()); !returnValue) {
			if (this->doWePrintError) {
				reportSSLError("SSL_set_tlsext_host_name() Error: ", returnValue, this->ssl);
			}
			return;
		}

		if (auto returnValue = SSL_connect(this->ssl); !returnValue) {
			if (this->doWePrintError) {
				reportSSLError("SSL_connect() Error: ", returnValue, this->ssl);
			}
			return;
		}
	};

	bool WebSocketSSLClient::processIO(int32_t waitTimeInMicroSeconds) noexcept {
#ifdef _WIN32
		fd_set writeSet{}, readSet{};
		int32_t nfds{ 0 };
		FD_ZERO(&writeSet);
		FD_ZERO(&readSet);

		bool writing{ false };
		if (this->outputBuffer.size() > 0 && !this->wantRead) {
			writing = true;
			FD_SET(this->theSocket, &writeSet);
			nfds = this->theSocket > nfds ? this->theSocket : nfds;
		} else {
			FD_SET(this->theSocket, &readSet);
			nfds = this->theSocket > nfds ? this->theSocket : nfds;
		}

		timeval checkTime{ .tv_usec = waitTimeInMicroSeconds };
		if (auto resultValue = select(nfds + 1, &readSet, &writeSet, nullptr, &checkTime); resultValue == SOCKET_ERROR) {
			if (this->doWePrintError) {
				reportError("select() Error: ", resultValue);
			}
			return false;
		} else if (resultValue == 0) {
			return true;
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
				if (this->doWePrintError) {
					reportError("epoll_ctl() Error: ", resultValue);
				}
				return false;
			}
		} else {
			readEvent.events = EPOLLIN;
			readEvent.data.fd = this->theSocket;
			if (auto resultValue = epoll_ctl(epoll, EPOLL_CTL_ADD, this->theSocket, &readEvent); resultValue == SOCKET_ERROR) {
				if (this->doWePrintError) {
					reportError("epoll_ctl() Error: ", resultValue);
				}
				return false;
			}
		}

		if (auto resultValue = epoll_wait(epoll, events, 1, waitTimeInMicroSeconds / 1000); resultValue == SOCKET_ERROR) {
			if (this->doWePrintError) {
				reportError("epoll_wait() Error: ", resultValue);
			}
			return false;
		} else if (resultValue == 0) {
			return true;
		}

#endif

#ifdef _WIN32
		if (FD_ISSET(this->theSocket, &writeSet)) {
#else
		if (writing) {
#endif
			this->wantRead = false;
			size_t writtenBytes{ 0 };
			auto returnValue{ SSL_write_ex(this->ssl, this->outputBuffer.data(), this->outputBuffer.size(), &writtenBytes) };
			auto errorValue{ SSL_get_error(this->ssl, returnValue) };
			switch (errorValue) {
				case SSL_ERROR_NONE: {
					this->outputBuffer.clear();
					return true;
				}
				case SSL_ERROR_SYSCALL: {
					if (this->doWePrintError) {
						reportSSLError("WebSocketSSLClient::processIO::SSL_write_ex() Error: ", returnValue, this->ssl);
						reportError("WebSocketSSLClient::processIO::SSL_write_ex() Error: ", returnValue);
					}
					[[fallthrough]];
				}
				case SSL_ERROR_ZERO_RETURN: {
					return false;
				}
				case SSL_ERROR_WANT_READ: {
					this->wantRead = true;
					[[fallthrough]];
				}
				case SSL_ERROR_WANT_WRITE: {
					return true;
				}
				default: {
					if (this->doWePrintError) {
						reportSSLError("WebSocketSSLClient::processIO::SSL_write_ex() Error: ", returnValue, this->ssl);
						reportError("WebSocketSSLClient::processIO::SSL_write_ex() Error: ", returnValue);
					}
					return false;
				}
			}
		}
#ifdef _WIN32
		else if (FD_ISSET(this->theSocket, &readSet)) {
#else
		else {
#endif
			this->wantRead = false;
			std::string serverToClientBuffer{};
			serverToClientBuffer.resize(this->maxBufferSize);
			size_t readBytes{ 0 };
			auto returnValue{ SSL_read_ex(this->ssl, serverToClientBuffer.data(), this->maxBufferSize, &readBytes) };
			auto errorValue{ SSL_get_error(this->ssl, returnValue) };
			switch (errorValue) {
				case SSL_ERROR_NONE: {
					if (readBytes > 0) {
						this->inputBuffer.insert(this->inputBuffer.end(), serverToClientBuffer.begin(), serverToClientBuffer.begin() + readBytes);
						this->bytesRead += readBytes;
					}
					return true;
				}
				case SSL_ERROR_SYSCALL: {
					if (this->doWePrintError) {
						reportSSLError("WebSocketSSLClient::processIO::SSL_read_ex() Error: ", returnValue, this->ssl);
						reportError("WebSocketSSLClient::processIO::SSL_read_ex() Error: ", returnValue);
					}
					[[fallthrough]];
				}
				case SSL_ERROR_ZERO_RETURN: {
					return false;
				}
				case SSL_ERROR_WANT_READ: {
					this->wantRead = true;
					[[fallthrough]];
				}
				case SSL_ERROR_WANT_WRITE: {
					return true;
				}
				default: {
					if (this->doWePrintError) {
						reportSSLError("WebSocketSSLClient::processIO::SSL_read_ex() Error: ", returnValue, this->ssl);
						reportError("WebSocketSSLClient::processIO::SSL_read_ex() Error: ", returnValue);
					}
					return false;
				}
			}
		}
		return true;
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

	DatagramSocketSSLClient::DatagramSocketSSLClient(const std::string& baseUrlNew, const std::string& portNew, bool doWePrintErrorNew) noexcept {
		this->doWePrintError = doWePrintErrorNew;
		addrinfoWrapper resultAddress{ nullptr }, hints{ nullptr };
		hints->ai_family = AF_INET;
		hints->ai_socktype = SOCK_DGRAM;
		hints->ai_protocol = IPPROTO_UDP;

		if (auto returnValue = getaddrinfo(baseUrlNew.c_str(), portNew.c_str(), hints, resultAddress); returnValue == SOCKET_ERROR) {
			if (this->doWePrintError) {
				reportError("getaddrinfo() Error: ", returnValue);
			}
			return;
		}

		if (this->theSocket = socket(resultAddress->ai_family, resultAddress->ai_socktype, resultAddress->ai_protocol); this->theSocket == SOCKET_ERROR) {
			if (this->doWePrintError) {
				reportError("socket() Error: ", 0);
			}
			return;
		}

		if (auto returnValue = connect(this->theSocket, resultAddress->ai_addr, static_cast<int32_t>(resultAddress->ai_addrlen)); returnValue == SOCKET_ERROR) {
			if (this->doWePrintError) {
				reportError("connect() Error: ", returnValue);
			}
			return;
		}

#ifdef _WIN32
		u_long value{ 1 };
		if (auto returnValue = ioctlsocket(this->theSocket, FIONBIO, &value); returnValue == SOCKET_ERROR) {
			if (this->doWePrintError) {
				reportError("ioctlsocket() Error: ", returnValue);
			}
			return;
		}
#else
		if (auto returnValue = fcntl(this->theSocket, F_SETFL, fcntl(this->theSocket, F_GETFL, 0) | O_NONBLOCK); returnValue == SOCKET_ERROR) {
			if (this->doWePrintError) {
				reportError("fcntl() Error: ", returnValue);
			}
			return;
		}
#endif
		if (this->datagramBio = BIO_new_dgram(this->theSocket, BIO_CLOSE); this->datagramBio == nullptr) {
			if (this->doWePrintError) {
				reportSSLError("BIO_new_dgram() Error: ");
			}
			return;
		}

		if (auto returnValue = BIO_ctrl(this->datagramBio, BIO_CTRL_DGRAM_SET_CONNECTED, 0, &resultAddress); returnValue == 0) {
			if (this->doWePrintError) {
				reportSSLError("BIO_ctrl() Error: ");
			}
			return;
		}

		return;
	}

	bool DatagramSocketSSLClient::writeData(std::string& data) noexcept {
		size_t writtenBytes{ 0 };
		if (!BIO_write_ex(this->datagramBio, data.data(), data.size(), &writtenBytes)) {
			if (this->doWePrintError) {
				reportSSLError("BIO_write_ex() Error: ");
			}
			return false;
		}
		data.clear();
		return true;
	}

	std::string& DatagramSocketSSLClient::getInputBuffer() noexcept {
		return this->inputBuffer;
	}

	void DatagramSocketSSLClient::readData(bool doWeClear) noexcept {
		std::string serverToClientBuffer{};
		serverToClientBuffer.resize(this->maxBufferSize);
		size_t readBytes{ 0 };
		if (auto returnValue = BIO_read_ex(this->datagramBio, serverToClientBuffer.data(), this->maxBufferSize, &readBytes); returnValue == 1) {
			if (readBytes > 0) {
				this->inputBuffer.insert(this->inputBuffer.end(), serverToClientBuffer.begin(), serverToClientBuffer.begin() + readBytes);
				if (doWeClear) {
					this->inputBuffer.clear();
				}
			}
		}
	}

	std::string HttpSSLClient::soundcloudCertPathStatic{};
	std::string HttpSSLClient::defaultCertPathStatic{};
	std::string HttpSSLClient::googleCertPathStatic{};
	std::mutex HttpSSLClient::theMutex{};
}
