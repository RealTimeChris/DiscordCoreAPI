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
/// Chris M.
/// https://discordcoreapi.com
/// \file SSLClients.cpp

#include <discordcoreapi/SSLClients.hpp>

namespace DiscordCoreInternal {

	auto getFilePath(std::string fileName, std::string pathPrefix, std::string searchRoot = "") {
		{
			if (searchRoot == "") {
#ifdef _WIN32
				searchRoot = "C:\\";
#else
				searchRoot = "/";
#endif
			}
			try {
				for (const auto& entry: std::filesystem::recursive_directory_iterator(searchRoot, std::filesystem::directory_options::skip_permission_denied)) {
					try {
						if (entry.path().string().find(pathPrefix) == std::string::npos) {
							continue;
						} else if (entry.path().string().find(pathPrefix) != std::string::npos) {
							if (entry.path().string().find(fileName) != std::string::npos) {
								return entry.path().string();
							}
						}
					} catch (std::exception& e) {
						std::cout << e.what() << std::endl;
					}
				}
			} catch (std::exception& e) {
				std::cout << e.what() << std::endl;
			}
		}
		return std::string{};
	}

	void reportSSLError(std::string errorPosition, int32_t errorValue = 0, SSL* ssl = nullptr) noexcept {
		try {
			if (ssl) {
				std::cout << DiscordCoreAPI::shiftToBrightRed() << errorPosition << SSL_get_error(ssl, errorValue) << std::endl;
			} else {
				std::cout << DiscordCoreAPI::shiftToBrightRed() << errorPosition << std::endl;
			}
			ERR_print_errors_fp(stdout);
			std::cout << std::endl << DiscordCoreAPI::reset();
		} catch (...) {
			DiscordCoreAPI::reportException("reportSSLError()");
		}
	}

	void reportError(std::string errorPosition, int32_t errorValue) noexcept {
		try {
			std::cout << DiscordCoreAPI::shiftToBrightRed() << errorPosition << errorValue << ", ";
#ifdef _WIN32
			std::cout << WSAGetLastError() << std::endl << DiscordCoreAPI::reset();
#else
			std::cout << strerror(errno) << std::endl << DiscordCoreAPI::reset();
#endif
		} catch (...) {
			DiscordCoreAPI::reportException("reportError()");
		}
	}

	void HttpSSLClient::initialize() {
		try {
#ifdef _WIN32
			HttpSSLClient::googleCertPathStatic = getFilePath("GoogleCert.pem", "share\\discordcoreapi\\data");
			HttpSSLClient::defaultCertPathStatic = getFilePath("DiscordCert.pem", "share\\discordcoreapi\\data",
				HttpSSLClient::googleCertPathStatic.substr(0, HttpSSLClient::googleCertPathStatic.find("GoogleCert.pem")));
			HttpSSLClient::soundcloudCertPathStatic = getFilePath("SoundCloudCert.pem", "share\\discordcoreapi\\data",
				HttpSSLClient::googleCertPathStatic.substr(0, HttpSSLClient::googleCertPathStatic.find("GoogleCert.pem")));
#else
			HttpSSLClient::googleCertPathStatic = getFilePath("GoogleCert.pem", "share/discordcoreapi/data");
			HttpSSLClient::defaultCertPathStatic = getFilePath("DiscordCert.pem", "share/discordcoreapi/data",
				HttpSSLClient::googleCertPathStatic.substr(0, HttpSSLClient::googleCertPathStatic.find("GoogleCert.pem")));
			HttpSSLClient::soundcloudCertPathStatic = getFilePath("SoundCloudCert.pem", "share/discordcoreapi/data",
				HttpSSLClient::googleCertPathStatic.substr(0, HttpSSLClient::googleCertPathStatic.find("GoogleCert.pem")));
#endif
		} catch (...) {
			DiscordCoreAPI::reportException("HttpSSLClient::HttpSSLClient()");
		}
	}

	bool HttpSSLClient::connect(std::string& baseUrl, std::string portNew) noexcept {
		try {
			std::string stringNew{};
			if (baseUrl.find(".com") != std::string::npos) {
				stringNew =
					baseUrl.substr(baseUrl.find("https://") + std::string("https://").size(), baseUrl.find(".com") + std::string(".com").size() - std::string("https://").size());
			} else if (baseUrl.find(".org") != std::string::npos) {
				stringNew =
					baseUrl.substr(baseUrl.find("https://") + std::string("https://").size(), baseUrl.find(".org") + std::string(".org").size() - std::string("https://").size());
			}

			std::string certPath{};
			if (stringNew.find("soundcloud") != std::string::npos || stringNew.find("sndcdn") != std::string::npos) {
				certPath = HttpSSLClient::soundcloudCertPathStatic;
			} else if (stringNew.find("youtube") != std::string::npos || stringNew.find("google") != std::string::npos) {
				certPath = HttpSSLClient::googleCertPathStatic;
			} else {
				certPath = HttpSSLClient::defaultCertPathStatic;
			}

			if (this->context = SSL_CTX_new(TLS_client_method()); this->context == nullptr) {
				reportSSLError("SSL_CTX_new() Error: ");
				return false;
			}

			auto options{ SSL_CTX_get_options(this->context) };
			if (SSL_CTX_set_options(this->context, SSL_OP_IGNORE_UNEXPECTED_EOF) != (options | SSL_OP_IGNORE_UNEXPECTED_EOF)) {
				reportSSLError("SSL_CTX_set_options() Error: ");
				return false;
			}

			SSL_CTX_set_verify(this->context, SSL_VERIFY_PEER, nullptr);
			SSL_CTX_set_verify_depth(this->context, 4);

			std::unique_lock<std::mutex> theLock{ HttpSSLClient::theMutex };
			if (!SSL_CTX_load_verify_locations(this->context, certPath.c_str(), NULL)) {
				reportSSLError("SSL_CTX_load_verify_locations() Error: ");
				return false;
			}
			theLock.unlock();

			if (!SSL_CTX_set_cipher_list(this->context, "ALL")) {
				reportSSLError("SSL_CTX_set_cipher_list() Error: ");
				return false;
			}

			/* Do not allow SSL 3.0, TLS 1.0 or 1.1
			 * https://www.packetlabs.net/posts/tls-1-1-no-longer-secure/
			 */
			if (!SSL_CTX_set_min_proto_version(this->context, TLS1_2_VERSION)) {
				reportSSLError("SSL_CTX_set_min_proto_version() Error: ");
				return false;
			}

			if (!SSL_CTX_set_ciphersuites(this->context, "TLS_AES_256_GCM_SHA384:TLS_CHACHA20_POLY1305_SHA256:TLS_AES_128_GCM_SHA256")) {
				reportSSLError("SSL_CTX_set_ciphersuites() Error: ");
				return false;
			}

			if (this->connectionBio = BIO_new_ssl_connect(this->context); this->connectionBio == nullptr) {
				reportSSLError("BIO_new_ssl_connect() Error: ");
				return false;
			}

			if (!BIO_set_conn_hostname(this->connectionBio, std::string(stringNew + ":" + portNew).c_str())) {
				reportSSLError("BIO_set_conn_hostname() Error: ");
				return false;
			}

			if (BIO_get_ssl(this->connectionBio, &this->ssl); this->ssl == nullptr) {
				reportSSLError("BIO_get_ssl() Error: ");
				return false;
			}

			if (auto returnValue = SSL_set_tlsext_host_name(this->ssl, stringNew.c_str()); !returnValue) {
				reportSSLError("SSL_set_tlsext_host_name() Error: ", returnValue, this->ssl);
				return false;
			}

			if (auto returnValue = SSL_connect(this->ssl); !returnValue) {
				reportSSLError("SSL_connect() Error: ", returnValue, this->ssl);
				return false;
			}

			if (std::unique_ptr<X509, X509Deleter> cert{ SSL_get1_peer_certificate(this->ssl) }; cert == nullptr) {
				reportSSLError("SSL_get_peer_certificate() Error: ", 0, this->ssl);
				return false;
			}

			if (auto returnValue = SSL_get_verify_result(this->ssl); returnValue != X509_V_OK) {
				reportSSLError("SSL_get_verify_result() Error: ", returnValue, this->ssl);
				return false;
			}

			if (this->theSocket = SSL_get_fd(this->ssl); this->theSocket == INVALID_SOCKET) {
				reportSSLError("SSL_get_fd() Error: ", static_cast<int32_t>(this->theSocket), this->ssl);
				return false;
			}
			return true;
		} catch (...) {
			DiscordCoreAPI::reportException("HttpSSLClient::connect()");
		}
		return false;
	}

	void HttpSSLClient::writeData(std::string& data) noexcept {
		try {
			this->outputBuffer.insert(this->outputBuffer.end(), data.begin(), data.end());
			data.clear();
		} catch (...) {
			DiscordCoreAPI::reportException("HttpSSLClient::writeData()");
		}
	}

	std::string& HttpSSLClient::getInputBuffer() noexcept {
		try {
			return this->inputBuffer;
		} catch (...) {
			DiscordCoreAPI::reportException("HttpSSLClient::getInputBuffer()");
		}
	}

	bool HttpSSLClient::processIO() noexcept {
		try {
#ifdef _WIN32
			fd_set writeSet{}, readSet{};
			uint64_t nfds{ 0 };
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
			if (auto resultValue = select(static_cast<int32_t>(nfds + 1), &readSet, &writeSet, nullptr, &checkTime); resultValue == SOCKET_ERROR) {
				reportError("select() Error: ", resultValue);
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
					reportError("epoll_ctl() Error: ", resultValue);
					return false;
				}
			} else {
				readEvent.events = EPOLLIN;
				readEvent.data.fd = this->theSocket;
				if (auto resultValue = epoll_ctl(epoll, EPOLL_CTL_ADD, this->theSocket, &readEvent); resultValue == SOCKET_ERROR) {
					reportError("epoll_ctl() Error: ", resultValue);
					return false;
				}
			}

			if (auto resultValue = epoll_wait(epoll, events, 1, 1); resultValue == SOCKET_ERROR) {
				reportError("epoll_wait() Error: ", resultValue);
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
						reportSSLError("HttpSSLClient::processIO::SSL_write_ex() Error: ", returnValue, this->ssl);
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
						reportSSLError("HttpSSLClient::processIO::SSL_write_ex() Error: ", returnValue, this->ssl);
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
						reportSSLError("HttpSSLClient::processIO::SSL_read_ex() Error: ", returnValue, this->ssl);
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
						reportSSLError("HttpSSLClient::processIO::SSL_read_ex() Error: ", returnValue, this->ssl);
						return false;
					}
				}
			}
			return true;
		} catch (...) {
			DiscordCoreAPI::reportException("HttpSSLClient::processIO()");
			return false;
		}
	}

	WebSocketSSLClient::WebSocketSSLClient(std::string& baseUrlNew, std::string portNew, int64_t maxBufferSizeNew) noexcept : maxBufferSize(maxBufferSizeNew) {
		try {
			addrinfoWrapper resultAddress{ nullptr }, hints{ nullptr };
			hints->ai_family = AF_INET;
			hints->ai_socktype = SOCK_STREAM;
			hints->ai_protocol = IPPROTO_TCP;

			if (auto returnValue = getaddrinfo(baseUrlNew.c_str(), portNew.c_str(), hints, resultAddress); returnValue == SOCKET_ERROR) {
				reportError("getaddrinfo() Error: ", returnValue);
				return;
			}

			if (this->theSocket = socket(resultAddress->ai_family, resultAddress->ai_socktype, resultAddress->ai_protocol); this->theSocket == INVALID_SOCKET) {
				reportError("socket() Error: ", static_cast<int32_t>(this->theSocket));
				return;
			}

			if (auto returnValue = connect(this->theSocket, resultAddress->ai_addr, static_cast<int32_t>(resultAddress->ai_addrlen)); returnValue == SOCKET_ERROR) {
				reportError("connect() Error: ", returnValue);
				return;
			}

#ifdef _WIN32
			char optionValue{ true };
			if (auto returnValue = setsockopt(this->theSocket, IPPROTO_TCP, TCP_NODELAY, &optionValue, sizeof(optionValue)); returnValue == SOCKET_ERROR) {
				reportError("setsockopt() Error: ", returnValue);
				return;
			}
#else
			int32_t optionValue{ 1 };
			if (auto returnValue = setsockopt(this->theSocket, SOL_TCP, TCP_NODELAY, &optionValue, sizeof(optionValue)); returnValue == SOCKET_ERROR) {
				reportError("setsockopt() Error: ", returnValue);
				return;
			}
#endif

			if (this->context = SSL_CTX_new(TLS_client_method()); this->context == nullptr) {
				reportSSLError("SSL_CTX_new() Error: ");
				return;
			}

			auto options{ SSL_CTX_get_options(this->context) };
			if (SSL_CTX_set_options(this->context, SSL_OP_IGNORE_UNEXPECTED_EOF) != (options | SSL_OP_IGNORE_UNEXPECTED_EOF)) {
				reportSSLError("SSL_CTX_set_options() Error: ");
				return;
			}

			if (this->ssl = SSL_new(this->context); this->ssl == nullptr) {
				reportSSLError("SSL_new() Error: ");
				return;
			}

			if (auto returnValue = SSL_set_fd(this->ssl, static_cast<int32_t>(this->theSocket)); !returnValue) {
				reportSSLError("SSL_set_fd() Error: ", returnValue, this->ssl);
				return;
			}

			/* SNI */
			if (auto returnValue = SSL_set_tlsext_host_name(this->ssl, baseUrlNew.c_str()); !returnValue) {
				reportSSLError("SSL_set_tlsext_host_name() Error: ", returnValue, this->ssl);
				return;
			}

			if (auto returnValue = SSL_connect(this->ssl); !returnValue) {
				reportSSLError("SSL_connect() Error: ", returnValue, this->ssl);
				return;
			}
		} catch (...) {
			DiscordCoreAPI::reportException("WebSocketSSLClient::WebSocketSSLClient()");
		}
	};

	bool WebSocketSSLClient::processIO(int32_t waitTimeInMicroSeconds) noexcept {
		try {
#ifdef _WIN32
			fd_set writeSet{}, readSet{};
			uint64_t nfds{ 0 };
			FD_ZERO(&writeSet);
			FD_ZERO(&readSet);

			if (this->outputBuffer.size() > 0 && !this->wantRead) {
				FD_SET(this->theSocket, &writeSet);
				nfds = this->theSocket > nfds ? this->theSocket : nfds;
			} else {
				FD_SET(this->theSocket, &readSet);
				nfds = this->theSocket > nfds ? this->theSocket : nfds;
			}

			timeval checkTime{ .tv_usec = waitTimeInMicroSeconds };
			if (auto resultValue = select(static_cast<int32_t>(nfds + 1), &readSet, &writeSet, nullptr, &checkTime); resultValue == SOCKET_ERROR) {
				reportError("select() Error: ", resultValue);
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
					reportError("epoll_ctl() Error: ", resultValue);
					return false;
				}
			} else {
				readEvent.events = EPOLLIN;
				readEvent.data.fd = this->theSocket;
				if (auto resultValue = epoll_ctl(epoll, EPOLL_CTL_ADD, this->theSocket, &readEvent); resultValue == SOCKET_ERROR) {
					reportError("epoll_ctl() Error: ", resultValue);
					return false;
				}
			}

			if (auto resultValue = epoll_wait(epoll, events, 1, waitTimeInMicroSeconds / 1000); resultValue == SOCKET_ERROR) {
				reportError("epoll_wait() Error: ", resultValue);
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
						reportSSLError("WebSocketSSLClient::processIO::SSL_write_ex() Error: ", returnValue, this->ssl);
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
						reportSSLError("WebSocketSSLClient::processIO::SSL_write_ex() Error: ", returnValue, this->ssl);
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
						reportSSLError("WebSocketSSLClient::processIO::SSL_read_ex() Error: ", returnValue, this->ssl);
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
						reportSSLError("WebSocketSSLClient::processIO::SSL_read_ex() Error: ", returnValue, this->ssl);
						return false;
					}
				}
			}
			return true;
		} catch (...) {
			DiscordCoreAPI::reportException("WebSocketSSLClient::processIO()");
			return false;
		}
	}

	void WebSocketSSLClient::writeData(std::string& data) noexcept {
		try {
			this->outputBuffer.insert(this->outputBuffer.end(), data.begin(), data.end());
		} catch (...) {
			DiscordCoreAPI::reportException("WebSocketSSLClient::writeData()");
		}
	}

	std::string& WebSocketSSLClient::getInputBuffer() noexcept {
		try {
			return this->inputBuffer;
		} catch (...) {
			DiscordCoreAPI::reportException("WebSocketSSLClient::getInputBuffer()");
		}
	}

	int64_t WebSocketSSLClient::getBytesRead() noexcept {
		try {
			return this->bytesRead;
		} catch (...) {
			DiscordCoreAPI::reportException("WebSocketSSLClient::getBytesRead()");
		}
	}

	DatagramSocketSSLClient::DatagramSocketSSLClient(std::string& baseUrlNew, std::string& portNew) noexcept {
		try {
			addrinfoWrapper resultAddress{ nullptr }, hints{ nullptr };
			hints->ai_family = AF_INET;
			hints->ai_socktype = SOCK_DGRAM;
			hints->ai_protocol = IPPROTO_UDP;

			if (auto returnValue = getaddrinfo(baseUrlNew.c_str(), portNew.c_str(), hints, resultAddress); returnValue == SOCKET_ERROR) {
				reportError("getaddrinfo() Error: ", returnValue);
				return;
			}

			if (this->theSocket = socket(resultAddress->ai_family, resultAddress->ai_socktype, resultAddress->ai_protocol); this->theSocket == INVALID_SOCKET) {
				reportError("socket() Error: ", 0);
				return;
			}

			if (auto returnValue = connect(this->theSocket, resultAddress->ai_addr, static_cast<int32_t>(resultAddress->ai_addrlen)); returnValue == SOCKET_ERROR) {
				reportError("connect() Error: ", returnValue);
				return;
			}

#ifdef _WIN32
			u_long value{ 1 };
			if (auto returnValue = ioctlsocket(this->theSocket, FIONBIO, &value); returnValue == SOCKET_ERROR) {
				reportError("ioctlsocket() Error: ", returnValue);
				return;
			}
#else
			if (auto returnValue = fcntl(this->theSocket, F_SETFL, fcntl(this->theSocket, F_GETFL, 0) | O_NONBLOCK); returnValue == SOCKET_ERROR) {
				reportError("fcntl() Error: ", returnValue);
				return;
			}
#endif
			if (this->datagramBio = BIO_new_dgram(static_cast<int32_t>(this->theSocket), BIO_CLOSE); this->datagramBio == nullptr) {
				reportSSLError("BIO_new_dgram() Error: ");
				return;
			}

			if (auto returnValue = BIO_ctrl(this->datagramBio, BIO_CTRL_DGRAM_SET_CONNECTED, 0, &resultAddress); returnValue == 0) {
				reportSSLError("BIO_ctrl() Error: ");
				return;
			}

			return;
		} catch (...) {
			DiscordCoreAPI::reportException("DatagramSocketSSLClient::DatagramSocketSSLClient()");
		}
	}

	bool DatagramSocketSSLClient::writeData(std::string& data) noexcept {
		try {
			size_t writtenBytes{ 0 };

			if (!BIO_write_ex(this->datagramBio, data.data(), data.size(), &writtenBytes)) {
				reportSSLError("BIO_write_ex() Error: ");
				return false;
			}
			data.clear();
			return true;
		} catch (...) {
			DiscordCoreAPI::reportException("DatagramSocketSSLClient::writeData()");
			return false;
		}
	}

	std::string& DatagramSocketSSLClient::getInputBuffer() noexcept {
		try {
			return this->inputBuffer;
		} catch (...) {
			DiscordCoreAPI::reportException("DatagramSocketSSLClient::getInputBuffer()");
		}
	}

	void DatagramSocketSSLClient::readData(bool doWeClear) noexcept {
		try {
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
		} catch (...) {
			DiscordCoreAPI::reportException("DatagramSocketSSLClient::readData()");
		}
	}

	std::string HttpSSLClient::soundcloudCertPathStatic{};
	std::string HttpSSLClient::defaultCertPathStatic{};
	std::string HttpSSLClient::googleCertPathStatic{};
	std::mutex HttpSSLClient::theMutex{};
}
