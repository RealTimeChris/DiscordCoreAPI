/*
*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

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
#include <discordcoreapi/WebSocketEntities.hpp>

namespace DiscordCoreInternal {

	std::string reportSSLError(const std::string& errorPosition, int32_t errorValue = 0, SSL* ssl = nullptr) noexcept {
		std::stringstream theStream{};
		if (ssl) {
			theStream << DiscordCoreAPI::shiftToBrightRed() << errorPosition << SSL_get_error(ssl, errorValue) << ", " << ERR_error_string(errorValue, nullptr)
					  << DiscordCoreAPI::reset() << std::endl;
		} else {
			theStream << DiscordCoreAPI::shiftToBrightRed() << errorPosition << DiscordCoreAPI::reset() << std::endl;
		}

		return theStream.str();
	}

	std::string reportError(const std::string& errorPosition) noexcept {
		std::stringstream theStream{};
		theStream << DiscordCoreAPI::shiftToBrightRed() << errorPosition;
#ifdef _WIN32
		std::unique_ptr<char[]> string{ std::make_unique<char[]>(1024) };
	#ifdef UWP
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), ( LPWSTR )string.get(), 1024,
			NULL);
	#else
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), string.get(), 1024, NULL);
	#endif
		theStream << WSAGetLastError() << ", " << string << DiscordCoreAPI::reset();
#else
		theStream << strerror(errno) << DiscordCoreAPI::reset();
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

		if (auto returnValue = getaddrinfo(stringNew.c_str(), portNew.c_str(), hints, address); returnValue == SOCKET_ERROR) {
			throw ConnectionError{ reportError("HttpSSLClient::connect()::getaddrinfo(), ") };
		}

		if (this->theSocket = socket(address->ai_family, address->ai_socktype, address->ai_protocol); this->theSocket == SOCKET_ERROR) {
			throw ConnectionError{ reportError("HttpSSLClient::connect()::socket(), ") };
		}

		int32_t value{ this->maxBufferSize + 1 };
		if (auto returnValue = setsockopt(this->theSocket, SOL_SOCKET, SO_SNDBUF, static_cast<char*>(static_cast<void*>(&value)), sizeof(value)); returnValue == SOCKET_ERROR) {
			throw ConnectionError{ reportError("HttpSSLClient::connect()::setsockopt(), ") };
		}

#ifdef _WIN32
		char optionValue{ true };
		if (auto returnValue = setsockopt(this->theSocket, IPPROTO_TCP, TCP_NODELAY, &optionValue, sizeof(optionValue)); returnValue == SOCKET_ERROR) {
			throw ConnectionError{ reportError("HttpSSLClient::connect()::setsockopt(), ") };
		}
#else
		int32_t optionValue{ 1 };
		if (auto returnValue = setsockopt(this->theSocket, SOL_TCP, TCP_NODELAY, &optionValue, sizeof(optionValue)); returnValue == SOCKET_ERROR) {
			throw ConnectionError {
				reportError("HttpSSLClient::connect()::setsockopt(), ");
			}
#endif
		if (auto returnValue = ::connect(this->theSocket, address->ai_addr, static_cast<int32_t>(address->ai_addrlen)); returnValue == SOCKET_ERROR) {
			throw ConnectionError{ reportError("HttpSSLClient::connect()::connect(), ") };
		}

		if (this->context = SSL_CTX_new(TLS_client_method()); this->context == nullptr) {
			throw ConnectionError{ reportSSLError("HttpSSLClient::connect()::SSL_CTX_new(), ") };
		}

		if (!SSL_CTX_set_min_proto_version(this->context, TLS1_2_VERSION)) {
			throw ConnectionError{ reportSSLError("HttpSSLClient::connect()::SSL_CTX_set_min_proto_version(), ") };
		}

		if (SSL_CTX_set_options(this->context, SSL_OP_IGNORE_UNEXPECTED_EOF) != (SSL_CTX_get_options(this->context) | SSL_OP_IGNORE_UNEXPECTED_EOF)) {
			throw ConnectionError{ reportSSLError("HttpSSLClient::connect()::SSL_CTX_set_options(), ") };
		}

		if (this->ssl = SSL_new(this->context); this->ssl == nullptr) {
			throw ConnectionError{ reportSSLError("HttpSSLClient::connect()::SSL_new(), ") };
		}

		if (auto returnValue = SSL_set_fd(this->ssl, this->theSocket); !returnValue) {
			throw ConnectionError{ reportSSLError("HttpSSLClient::connect()::SSL_set_fd(), ", returnValue, this->ssl) };
		}

		/* SNI */
		if (auto returnValue = SSL_set_tlsext_host_name(this->ssl, stringNew.c_str()); !returnValue) {
			throw ConnectionError{ reportSSLError("HttpSSLClient::connect()::SSL_set_tlsext_host_name(), ", returnValue, this->ssl) };
		}

		if (auto returnValue = SSL_connect(this->ssl); !returnValue) {
			throw ConnectionError{ reportSSLError("HttpSSLClient::connect()::SSL_connect(), ", returnValue, this->ssl) };
		}
		
		this->connectionTime = std::time(nullptr);
	}

	void HttpSSLClient::processIO(int32_t theWaitTimeInms) {
		fd_set writeSet{}, readSet{};
		int32_t readNfds{ 0 }, writeNfds{ 0 }, finalNfds{ 0 };
		FD_ZERO(&writeSet);
		FD_ZERO(&readSet);

		if ((this->outputBuffer.size() > 0 || this->wantWrite) && !this->wantRead) {
			FD_SET(this->theSocket, &writeSet);
			writeNfds = this->theSocket > writeNfds ? this->theSocket : writeNfds;
		}
		FD_SET(this->theSocket, &readSet);
		readNfds = this->theSocket > readNfds ? this->theSocket : readNfds;
		finalNfds = readNfds > writeNfds ? readNfds : writeNfds;

		timeval checkTime{ .tv_usec = theWaitTimeInms };
		if (auto returnValue = select(finalNfds + 1, &readSet, &writeSet, nullptr, &checkTime); returnValue == SOCKET_ERROR) {
			throw ProcessingError{ reportError("HttpSSLClient::processIO()::select(), ") };
		} else if (returnValue == 0) {
			return;
		}

		if (FD_ISSET(this->theSocket, &readSet)) {
			this->wantRead = false;
			this->wantWrite = false;
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
					break;
				}
				case SSL_ERROR_WANT_READ: {
					this->wantRead = true;
					break;
				}
				case SSL_ERROR_WANT_WRITE: {
					this->wantWrite = true;
					break;
				}
				case SSL_ERROR_SYSCALL: {
					[[fallthrough]];
				}
				case SSL_ERROR_ZERO_RETURN: {
					[[fallthrough]];
				}
				default: {
					throw ProcessingError{ reportSSLError("HttpSSLClient::processIO()::SSL_read_ex(), ", errorValue, this->ssl) +
						reportError("HttpSSLClient::processIO()::SSL_read_ex(), ") };
				}
			}
		}
		if (FD_ISSET(this->theSocket, &writeSet)) {
			this->wantRead = false;
			this->wantWrite = false;
			size_t writtenBytes{ 0 };
			std::string writeString = std::move(this->outputBuffer.front());
			auto returnValue{ SSL_write_ex(this->ssl, writeString.data(), writeString.size(), &writtenBytes) };
			auto errorValue{ SSL_get_error(this->ssl, returnValue) };
			switch (errorValue) {
				case SSL_ERROR_NONE: {
					if (writtenBytes > 0) {
						this->outputBuffer.erase(this->outputBuffer.begin());
					} else {
						this->outputBuffer[0] = std::move(writeString);
					}
					break;
				}
				case SSL_ERROR_WANT_READ: {
					this->wantRead = true;
					break;
				}
				case SSL_ERROR_WANT_WRITE: {
					this->wantWrite = true;
					break;
				}
				case SSL_ERROR_SYSCALL: {
					[[fallthrough]];
				}
				case SSL_ERROR_ZERO_RETURN: {
					[[fallthrough]];
				}
				default: {
					throw ProcessingError{ reportSSLError("HttpSSLClient::processIO()::SSL_write_ex(), ", errorValue, this->ssl) +
						reportError("HttpSSLClient::processIO()::SSL_write_ex(), ") };
				}
			}
		}
	}

	void HttpSSLClient::writeData(std::string& data) noexcept {
		if (data.size() > static_cast<size_t>(16 * 1024)) {
			size_t remainingBytes{ data.size() };
			while (remainingBytes > 0) {
				std::string newString{};
				size_t amountToCollect{};
				if (data.size() >= static_cast<size_t>(1024 * 16)) {
					amountToCollect = static_cast<size_t>(1024 * 16);
				} else {
					amountToCollect = data.size();
				}
				newString.insert(newString.begin(), data.begin(), data.begin() + amountToCollect);
				this->outputBuffer.push_back(newString);
				data.erase(data.begin(), data.begin() + amountToCollect);
				remainingBytes = data.size();
			}
		} else {
			this->outputBuffer.push_back(data);
		}
	}

	std::string HttpSSLClient::getInputBuffer() noexcept {
		std::string theReturnString = this->inputBuffer;
		this->inputBuffer.clear();
		return theReturnString;
	}

	bool HttpSSLClient::areWeStillConnected() noexcept {
		fd_set errorfds{};
		int32_t nfds{};
		FD_ZERO(&errorfds);
		if (this->theSocket != SOCKET_ERROR) {
			FD_SET(this->theSocket, &errorfds);
			nfds = this->theSocket;
			timeval checkTime{ .tv_usec = 1 };
			if (auto returnValue = select(nfds + 1, nullptr, nullptr, &errorfds, &checkTime); returnValue == SOCKET_ERROR) {
				return false;
			}
			if (time(nullptr) - this->connectionTime > 60) {
				return false;
			}
			if (FD_ISSET(this->theSocket, &errorfds)) {
				return false;
			}
			return true;
		} else {
			return false;
		}
	}

	void HttpSSLClient::disconnect() noexcept {
		if (this->ssl != nullptr) {
			SSL_shutdown(this->ssl);
			SSL_free(this->ssl);
		}
		if (this->theSocket != SOCKET_ERROR) {
#ifdef _WIN32
			shutdown(this->theSocket, SD_BOTH);
			closesocket(this->theSocket);
#else
			shutdown(this->theSocket, SHUT_RDWR);
			close(this->theSocket);
#endif
		}
		this->theSocket = SOCKET_ERROR;
		this->outputBuffer.clear();
		this->inputBuffer.clear();
	}

	WebSocketSSLShard::WebSocketSSLShard(std::queue<DiscordCoreAPI::ConnectionPackage>* connectionsNew, int32_t currentBaseSocketAgentNew, int32_t currentShardNew,
		int32_t totalShardsNew, bool doWePrintErrorsNew, DiscordCoreAPI::TextFormat theFormat) noexcept	{
		this->heartBeatStopWatch = DiscordCoreAPI::StopWatch<std::chrono::milliseconds>{ 10000ms };
		this->currentBaseSocketAgent = currentBaseSocketAgentNew;
		this->doWePrintErrors = doWePrintErrorsNew;
		this->shard.push_back(currentShardNew);
		this->heartBeatStopWatch.resetTimer();
		this->shard.push_back(totalShardsNew);
		this->connections = connectionsNew;
		if (theFormat == DiscordCoreAPI::TextFormat::Etf) {
			this->dataOpCode = WebSocketOpCode::Op_Binary;
		} else {
			this->dataOpCode = WebSocketOpCode::Op_Text;
		}
	};

	void WebSocketSSLShard::processIO(std::unordered_map<SOCKET, std::unique_ptr<WebSocketSSLShard>>& theMap, int32_t waitTimeInms) {
		int32_t readNfds{ 0 }, writeNfds{ 0 }, finalNfds{ 0 };
		fd_set readSet{}, writeSet{};
		FD_ZERO(&readSet);
		FD_ZERO(&writeSet);
		bool didWeSetASocket{ false };
		for (auto& [key, value]: theMap) {
			if (value != nullptr) {
				if (value->theSocket != SOCKET_ERROR) {
					if ((value->outputBuffer.size() > 0 || value->wantWrite) && !value->wantRead) {
						FD_SET(value->theSocket, &writeSet);
						writeNfds = value->theSocket > writeNfds ? value->theSocket : writeNfds;
						didWeSetASocket = true;
					} else if (!value->wantWrite) {
						FD_SET(value->theSocket, &readSet);
						readNfds = value->theSocket > readNfds ? value->theSocket : readNfds;
						didWeSetASocket = true;
					}
					finalNfds = readNfds > writeNfds ? readNfds : writeNfds;
				}
			}
		}

		if (!didWeSetASocket) {
			return;
		}

		timeval checkTime{ .tv_usec = waitTimeInms };
		if (auto returnValue = select(finalNfds + 1, &readSet, &writeSet, nullptr, &checkTime); returnValue == SOCKET_ERROR) {
			throw ConnectionError{ reportError("WebSocketSSLShard::processIO()::select(), ") };
		}

		for (auto& [key, value]: theMap) {
			if (FD_ISSET(value->theSocket, &readSet)) {
				value->wantRead = false;
				value->wantWrite = false;
				std::string serverToClientBuffer{};
				serverToClientBuffer.resize(value->maxBufferSize);
				size_t readBytes{ 0 };
				auto returnValue{ SSL_read_ex(value->ssl, serverToClientBuffer.data(), value->maxBufferSize, &readBytes) };
				auto errorValue{ SSL_get_error(value->ssl, returnValue) };
				switch (errorValue) {
					case SSL_ERROR_NONE: {
						if (readBytes > 0) {
							value->inputBuffer.insert(value->inputBuffer.end(), serverToClientBuffer.begin(), serverToClientBuffer.begin() + readBytes);
							value->bytesRead += readBytes;
						}
						break;
					}
					case SSL_ERROR_WANT_READ: {
						value->wantRead = true;
						break;
					}
					case SSL_ERROR_WANT_WRITE: {
						value->wantWrite = true;
						break;
					}
					case SSL_ERROR_SYSCALL: {
						[[fallthrough]];
					}
					case SSL_ERROR_ZERO_RETURN: {
						[[fallthrough]];
					}
					default: {
						value->reconnect();
						throw ProcessingError{ reportSSLError("Shard [" + std::to_string(key) + "], in WebSocketSSLShard::processIO()::SSL_read_ex(), ", errorValue, value->ssl) +
							reportError("Shard [" + std::to_string(key) + "], in WebSocketSSLShard::processIO()::SSL_read_ex(), ") };
					}
				}
			}
			if (FD_ISSET(value->theSocket, &writeSet)) {
				value->wantRead = false;
				value->wantWrite = false;
				size_t writtenBytes{ 0 };
				std::string theString{};
				if (value->outputBuffer.size() > 0) {
					theString = std::move(value->outputBuffer.front());
				}
				if (theString.size() > 0) {
					auto returnValue{ SSL_write_ex(value->ssl, theString.data(), theString.size(), &writtenBytes) };
					auto errorValue{ SSL_get_error(value->ssl, returnValue) };
					switch (errorValue) {
						case SSL_ERROR_NONE: {
							if (value->outputBuffer.size() > 0 && writtenBytes > 0) {
								value->outputBuffer.erase(value->outputBuffer.begin());
							}
							break;
						}
						case SSL_ERROR_WANT_READ: {
							value->wantRead = true;
							break;
						}
						case SSL_ERROR_WANT_WRITE: {
							value->wantWrite = true;
							break;
						}
						case SSL_ERROR_SYSCALL: {
							[[fallthrough]];
						}
						case SSL_ERROR_ZERO_RETURN: {
							[[fallthrough]];
						}
						default: {
							value->reconnect();
							throw ProcessingError{ reportSSLError("Shard [" + std::to_string(key) + "], in WebSocketSSLShard::processIO()::SSL_write_ex(), ", errorValue,
													   value->ssl) +
								reportError("Shard [" + std::to_string(key) + "], in WebSocketSSLShard::processIO()::SSL_write_ex(), ") };
						}
					}
				}
			}
		}
	}

	void WebSocketSSLShard::connect(const std::string& baseUrlNew, const std::string& portNew) {
		addrinfoWrapper hints{ nullptr }, address{ nullptr };

		hints->ai_family = AF_INET;
		hints->ai_socktype = SOCK_STREAM;
		hints->ai_protocol = IPPROTO_TCP;

		if (auto returnValue = getaddrinfo(baseUrlNew.c_str(), portNew.c_str(), hints, address); returnValue == SOCKET_ERROR) {
			throw ConnectionError{ reportError("WebSocketSSLShard::connect()::getaddrinfo(), ") };
		}

		if (this->theSocket = socket(address->ai_family, address->ai_socktype, address->ai_protocol); this->theSocket == SOCKET_ERROR) {
			throw ConnectionError{ reportError("WebSocketSSLShard::connect()::socket(), ") };
		}

		int32_t value{ this->maxBufferSize + 1 };
		if (auto returnValue = setsockopt(this->theSocket, SOL_SOCKET, SO_SNDBUF, static_cast<char*>(static_cast<void*>(&value)), sizeof(value)); returnValue == SOCKET_ERROR) {
			throw ConnectionError{ reportError("WebSocketSSLShard::connect()::setsockopt(), ") };
		}

#ifdef _WIN32
		char optionValue{ true };
		if (auto returnValue = setsockopt(this->theSocket, IPPROTO_TCP, TCP_NODELAY, &optionValue, sizeof(optionValue)); returnValue == SOCKET_ERROR) {
			throw ConnectionError{ reportError("WebSocketSSLShard::connect()::setsockopt(), ") };
		}
#else
			int32_t optionValue{ 1 };
			if (auto returnValue = setsockopt(this->theSocket, SOL_TCP, TCP_NODELAY, &optionValue, sizeof(optionValue)); returnValue == SOCKET_ERROR) {
				throw ConnectionError{ reportError("WebSocketSSLShard::connect()::setsockopt(), ") };
			}
#endif
		if (auto returnValue = ::connect(this->theSocket, address->ai_addr, static_cast<int32_t>(address->ai_addrlen)); returnValue == SOCKET_ERROR) {
			throw ConnectionError{ reportError("WebSocketSSLShard::connect()::connect(), ") };
		}

		if (this->context = SSL_CTX_new(TLS_client_method()); this->context == nullptr) {
			throw ConnectionError{ reportSSLError("WebSocketSSLShard::connect()::SSL_CTX_new(), ") };
		}

		if (!SSL_CTX_set_min_proto_version(this->context, TLS1_2_VERSION)) {
			throw ConnectionError{ reportSSLError("WebSocketSSLShard::connect()::SSL_CTX_set_min_proto_version(), ") };
		}

		if (SSL_CTX_set_options(this->context, SSL_OP_IGNORE_UNEXPECTED_EOF) != (SSL_CTX_get_options(this->context) | SSL_OP_IGNORE_UNEXPECTED_EOF)) {
			throw ConnectionError{ reportSSLError("WebSocketSSLShard::connect()::SSL_CTX_set_options(), ") };
		}

		if (this->ssl = SSL_new(this->context); this->ssl == nullptr) {
			throw ConnectionError{ reportSSLError("WebSocketSSLShard::connect()::SSL_new(), ") };
		}

		if (auto returnValue = SSL_set_fd(this->ssl, this->theSocket); !returnValue) {
			throw ConnectionError{ reportSSLError("WebSocketSSLShard::connect()::SSL_set_fd(), ", returnValue, this->ssl) };
		}

		/* SNI */
		if (auto returnValue = SSL_set_tlsext_host_name(this->ssl, baseUrlNew.c_str()); !returnValue) {
			throw ConnectionError{ reportSSLError("WebSocketSSLShard::connect()::SSL_set_tlsext_host_name(), ", returnValue, this->ssl) };
		}

		if (auto returnValue = SSL_connect(this->ssl); !returnValue) {
			throw ConnectionError{ reportSSLError("WebSocketSSLShard::connect()::SSL_connect(), ", returnValue, this->ssl) };
		}
	}

	void WebSocketSSLShard::writeData(std::string& data, bool priority) noexcept {
		if (priority) {
			size_t writtenBytes{ 0 };
			if (data.size() > 0) {
				auto returnValue{ SSL_write_ex(this->ssl, data.data(), data.size(), &writtenBytes) };
				auto errorValue{ SSL_get_error(this->ssl, returnValue) };
				switch (errorValue) {
					case SSL_ERROR_NONE: {
						if (this->outputBuffer.size() > 0 && writtenBytes > 0) {
							this->outputBuffer.erase(this->outputBuffer.begin());
						}
						break;
					}
					case SSL_ERROR_WANT_READ: {
						this->wantRead = true;
						break;
					}
					case SSL_ERROR_WANT_WRITE: {
						this->wantWrite = true;
						break;
					}
					case SSL_ERROR_SYSCALL: {
						[[fallthrough]];
					}
					case SSL_ERROR_ZERO_RETURN: {
						[[fallthrough]];
					}
					default: {
						this->reconnect();
					}
				}
			}
		} else {
			if (data.size() > static_cast<size_t>(16 * 1024)) {
				size_t remainingBytes{ data.size() };
				while (remainingBytes > 0) {
					std::string newString{};
					size_t amountToCollect{};
					if (data.size() >= static_cast<size_t>(1024 * 16)) {
						amountToCollect = static_cast<size_t>(1024 * 16);
					} else {
						amountToCollect = data.size();
					}
					newString.insert(newString.begin(), data.begin(), data.begin() + amountToCollect);
					this->outputBuffer.push_back(newString);
					data.erase(data.begin(), data.begin() + amountToCollect);
					remainingBytes = data.size();
				}
			} else {
				this->outputBuffer.push_back(data);
			}
		}
	}

	std::string WebSocketSSLShard::getInputBuffer() noexcept {
		std::string theReturnString = this->inputBuffer;
		this->inputBuffer.clear();
		return theReturnString;
	}

	int64_t WebSocketSSLShard::getBytesRead() noexcept {
		return this->bytesRead;
	}

	bool WebSocketSSLShard::areWeStillConnected() noexcept {
		fd_set errorfds{};
		int32_t nfds{};
		FD_ZERO(&errorfds);
		if (this->theSocket != SOCKET_ERROR) {
			FD_SET(this->theSocket, &errorfds);
			nfds = this->theSocket;
			timeval checkTime{ .tv_usec = 1 };
			if (auto returnValue = select(nfds + 1, nullptr, nullptr, &errorfds, &checkTime); returnValue == SOCKET_ERROR) {
				return false;
			}
			if (FD_ISSET(this->theSocket, &errorfds)) {
				return false;
			}
			return true;
		} else {
			return false;
		}
	}

	void WebSocketSSLShard::reconnect() noexcept {
		if (this->areWeConnected01.load()) {
			this->areWeConnected01.store(false);
			this->areWeConnected02.store(false);

			SSL_shutdown(this->ssl);
			SSL_free(this->ssl);
#ifdef _WIN32
			shutdown(this->theSocket, SD_BOTH);
			closesocket(this->theSocket);
#else
			shutdown(this->theSocket, SHUT_RDWR);
			close(this->theSocket);
#endif
			this->theSocket = SOCKET_ERROR;
			this->inputBuffer.clear();
			this->outputBuffer.clear();
			this->theState = WebSocketState::Connecting01;
			this->areWeHeartBeating = false;
			if (this->connections != nullptr) {
				DiscordCoreAPI::ConnectionPackage theData{};
				theData.currentBaseSocketAgent = this->currentBaseSocketAgent;
				theData.currentShard = this->shard[0];
				this->connections->push(theData);
			}
		}
	}

	void DatagramSocketSSLClient::connect(const std::string& baseUrlNew, const std::string& portNew) {
		addrinfoWrapper hints{ nullptr }, address{ nullptr };

		hints->ai_family = AF_INET;
		hints->ai_socktype = SOCK_DGRAM;
		hints->ai_protocol = IPPROTO_UDP;

		if (auto returnValue = getaddrinfo(baseUrlNew.c_str(), portNew.c_str(), hints, address); returnValue == SOCKET_ERROR) {
			throw ConnectionError{ reportError("DatagramSocketSSLClient::connect()::getaddrinfo(), ") };
		}

		if (this->theSocket = socket(address->ai_family, address->ai_socktype, address->ai_protocol); this->theSocket == SOCKET_ERROR) {
			throw ConnectionError{ reportError("DatagramSocketSSLClient::connect()::socket(), ") };
		}

		if (auto returnValue = ::connect(this->theSocket, address->ai_addr, static_cast<int32_t>(address->ai_addrlen)); returnValue == SOCKET_ERROR) {
			throw ConnectionError{ reportError("DatagramSocketSSLClient::connect()::connect(), ") };
		}

#ifdef _WIN32
		u_long value{ 1 };
		if (auto returnValue = ioctlsocket(this->theSocket, FIONBIO, &value); returnValue == SOCKET_ERROR) {
			throw ConnectionError{ reportError("DatagramSocketSSLClient::connect()::ioctlsocket(), ") };
		}
#else
			if (auto returnValue = fcntl(this->theSocket, F_SETFL, fcntl(this->theSocket, F_GETFL, 0) | O_NONBLOCK); returnValue == SOCKET_ERROR) {
				throw ConnectionError{ reportError("DatagramSocketSSLClient::connect()::fcntl(), ", this->theSocket) };
			}
#endif
		if (this->datagramBio = BIO_new_dgram(this->theSocket, BIO_CLOSE); this->datagramBio == nullptr) {
			throw ConnectionError{ reportSSLError("DatagramSocketSSLClient::connect()::BIO_new_dgram(), ") };
		}

		if (auto returnValue = BIO_ctrl(this->datagramBio, BIO_CTRL_DGRAM_SET_CONNECTED, 0, &address); returnValue == 0) {
			throw ConnectionError{ reportSSLError("DatagramSocketSSLClient::connect()::BIO_ctrl(), ") };
		}
	}

	void DatagramSocketSSLClient::writeData(const std::string& data) {
		size_t writtenBytes{ 0 };
		if (auto returnValue = BIO_write_ex(this->datagramBio, data.data(), data.size(), &writtenBytes); !returnValue) {
			throw ProcessingError{ reportSSLError("DatagramSocketSSLClient::writeData()::BIO_write_ex(), ", returnValue) + "\n" +
				reportError("DatagramSocketSSLClient::writeData()::BIO_write_ex(), ") };
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
		if (auto returnValue = BIO_read_ex(this->datagramBio, serverToClientBuffer.data(), this->maxBufferSize, &readBytes); returnValue == 1) {
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