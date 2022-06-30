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

	void SSLConnectionInterface::initialize() {
		if (SSLConnectionInterface::context = SSL_CTX_new(TLS_client_method()); SSLConnectionInterface::context == nullptr) {
			throw ConnectionError{ reportSSLError("SSLConnectionInterface::initialize()::SSL_CTX_new(), ") };
		}

		if (!SSL_CTX_set_min_proto_version(SSLConnectionInterface::context, TLS1_2_VERSION)) {
			throw ConnectionError{ reportSSLError("SSLConnectionInterface::initialize()::SSL_CTX_set_min_proto_version(), ") };
		}

		if (SSL_CTX_set_options(SSLConnectionInterface::context, SSL_OP_IGNORE_UNEXPECTED_EOF) !=
			(SSL_CTX_get_options(SSLConnectionInterface::context) | SSL_OP_IGNORE_UNEXPECTED_EOF)) {
			throw ConnectionError{ reportSSLError("SSLConnectionInterface::initialize()::SSL_CTX_set_options(), ") };
		}
	}

	bool HttpsSSLClient::writeData(const std::string& dataToWrite, bool priority) noexcept {
		std::string data = dataToWrite;
		if (priority && data.size() < (16 * 1024)) {
			size_t writtenBytes{ 0 };
			if (data.size() > 0 && this->ssl) {
				auto returnValue{ SSL_write_ex(this->ssl, data.data(), data.size(), &writtenBytes) };
				auto errorValue{ SSL_get_error(this->ssl, returnValue) };
				switch (errorValue) {
					case SSL_ERROR_NONE: {
						if (writtenBytes > 0) {
							data.clear();
							return true;
						}
						return false;
					}
					case SSL_ERROR_WANT_READ: {
						this->wantRead = true;
						return false;
					}
					case SSL_ERROR_WANT_WRITE: {
						this->wantWrite = true;
						return true;
					}
					default: {
						this->disconnect();
						return false;
					}
				}
			}
			return false;
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
					this->outputBuffers.push_back(newString);
					data.erase(data.begin(), data.begin() + amountToCollect);
					remainingBytes = data.size();
				}
			} else {
				this->outputBuffers.push_back(data);
			}
			return true;
		}
	}

	void HttpsSSLClient::processIO(int32_t theWaitTimeInms) {
		if (this->theSocket == SOCKET_ERROR) {
			this->disconnect();
			throw ProcessingError{ reportSSLError("HttpsSSLClient::processIO()::SSL_read_ex(), ") + reportError("HttpsSSLClient::processIO()::SSL_read_ex(), ") };
		}
		int32_t readNfds{ 0 }, writeNfds{ 0 }, finalNfds{ 0 };
		fd_set writeSet{}, readSet{};
		FD_ZERO(&writeSet);
		FD_ZERO(&readSet);
		if ((this->outputBuffers.size() > 0 || this->wantWrite) && !this->wantRead) {
			FD_SET(this->theSocket, &writeSet);
			writeNfds = this->theSocket > writeNfds ? this->theSocket : writeNfds;
		}
		FD_SET(this->theSocket, &readSet);
		readNfds = this->theSocket > readNfds ? this->theSocket : readNfds;
		finalNfds = readNfds > writeNfds ? readNfds : writeNfds;

		timeval checkTime{ .tv_usec = theWaitTimeInms };
		if (auto returnValue = select(finalNfds + 1, &readSet, &writeSet, nullptr, &checkTime); returnValue == SOCKET_ERROR) {
			throw ProcessingError{ reportError("HttpsSSLClient::processIO()::select(), ") };
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
				case SSL_ERROR_ZERO_RETURN: {
					this->disconnect();
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
				default: {
					this->connectionTime = 0;
					throw ProcessingError{ reportSSLError("HttpsSSLClient::processIO()::SSL_read_ex(), ", errorValue, this->ssl) +
						reportError("HttpsSSLClient::processIO()::SSL_read_ex(), ") };
				}
			}
		}
		if (FD_ISSET(this->theSocket, &writeSet)) {
			this->wantRead = false;
			this->wantWrite = false;
			size_t writtenBytes{ 0 };
			std::string writeString{};
			if (this->outputBuffers.size() > 0) {
				writeString = std::move(this->outputBuffers.front());
				auto returnValue{ SSL_write_ex(this->ssl, writeString.data(), writeString.size(), &writtenBytes) };
				auto errorValue{ SSL_get_error(this->ssl, returnValue) };
				switch (errorValue) {
					case SSL_ERROR_NONE: {
						if (writtenBytes > 0) {
							this->outputBuffers.erase(this->outputBuffers.begin());
						} else {
							this->outputBuffers[0] = std::move(writeString);
						}
						break;
					}
					case SSL_ERROR_ZERO_RETURN: {
						this->disconnect();
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
					default: {
						this->connectionTime = 0;
						throw ProcessingError{ reportSSLError("HttpsSSLClient::processIO()::SSL_write_ex(), ", errorValue, this->ssl) +
							reportError("HttpsSSLClient::processIO()::SSL_write_ex(), ") };
					}
				}
			}
		}
	}

	void HttpsSSLClient::connect(const std::string& baseUrl, const std::string& portNew) {
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
			throw ConnectionError{ reportError("HttpsSSLClient::connect()::getaddrinfo(), ") };
		}

		if (this->theSocket = socket(address->ai_family, address->ai_socktype, address->ai_protocol); this->theSocket == SOCKET_ERROR) {
			throw ConnectionError{ reportError("HttpsSSLClient::connect()::socket(), ") };
		}

		int32_t value{ this->maxBufferSize + 1 };
		if (setsockopt(this->theSocket, SOL_SOCKET, SO_SNDBUF, static_cast<char*>(static_cast<void*>(&value)), sizeof(value))) {
			throw ConnectionError{ reportError("HttpsSSLClient::connect()::setsockopt(), ") };
		}

		const char optionValue{ true };
		if (setsockopt(this->theSocket, IPPROTO_TCP, TCP_NODELAY, &optionValue, sizeof(int32_t))) {
			throw ConnectionError{ reportError("HttpsSSLClient::connect()::setsockopt(), ") };
		}

		linger optionValue02{};
		optionValue02.l_onoff = 0;
		if (setsockopt(this->theSocket, SOL_SOCKET, SO_LINGER, reinterpret_cast<char*>(&optionValue02), sizeof(linger))) {
			throw ConnectionError{ reportError("HttpsSSLClient::connect()::setsockopt(), ") };
		}

		if (setsockopt(this->theSocket, SOL_SOCKET, SO_KEEPALIVE, &optionValue, sizeof(int32_t))) {
			throw ConnectionError{ reportError("HttpsSSLClient::connect()::setsockopt(), ") };
		}

		if (::connect(this->theSocket, address->ai_addr, static_cast<int32_t>(address->ai_addrlen)) == SOCKET_ERROR) {
			throw ConnectionError{ reportError("HttpsSSLClient::connect()::connect(), ") };
		}

		std::unique_lock<std::mutex> theLock{ SSLConnectionInterface::theMutex };
		if (this->ssl = SSL_new(SSLConnectionInterface::context); this->ssl == nullptr) {
			throw ConnectionError{ reportSSLError("HttpSSLClient::connect()::SSL_new(), ") };
		}
		theLock.unlock();

		if (auto returnValue = SSL_set_fd(this->ssl, this->theSocket); !returnValue) {
			throw ConnectionError{ reportSSLError("HttpsSSLClient::connect()::SSL_set_fd(), ", returnValue, this->ssl) };
		}

		/* SNI */
		if (auto returnValue = SSL_set_tlsext_host_name(this->ssl, stringNew.c_str()); !returnValue) {
			throw ConnectionError{ reportSSLError("HttpsSSLClient::connect()::SSL_set_tlsext_host_name(), ", returnValue, this->ssl) };
		}

		if (auto returnValue = SSL_connect(this->ssl); !returnValue) {
			throw ConnectionError{ reportSSLError("HttpsSSLClient::connect()::SSL_connect(), ", returnValue, this->ssl) };
		}

#ifdef _WIN32
		u_long value02{ 1 };
		if (auto returnValue = ioctlsocket(this->theSocket, FIONBIO, &value02); returnValue == SOCKET_ERROR) {
			throw ConnectionError{ reportError("DatagramSocketSSLClient::connect()::ioctlsocket(), ") };
		}
#else
		if (auto returnValue = fcntl(this->theSocket, F_SETFL, fcntl(this->theSocket, F_GETFL, 0) | O_NONBLOCK); returnValue == SOCKET_ERROR) {
			throw ConnectionError{ reportError("DatagramSocketSSLClient::connect()::fcntl(), ") };
		}
#endif

		this->connectionTime = std::time(nullptr);
	}

	std::string HttpsSSLClient::getInputBuffer() noexcept {
		std::string theReturnString = this->inputBuffer;
		this->inputBuffer.clear();
		return theReturnString;
	}

	bool HttpsSSLClient::areWeStillConnected() noexcept {
		if (this->theSocket != SOCKET_ERROR) {
			int32_t nfds{};
			fd_set errorfds{};
			FD_ZERO(&errorfds);
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

	int64_t HttpsSSLClient::getBytesRead() noexcept {
		return this->bytesRead;
	}

	void HttpsSSLClient::disconnect() noexcept {
		this->areWeConnected01.store(false);
		this->areWeConnected02.store(false);
		this->theSocket = SOCKET_ERROR;
		this->inputBuffer.clear();
		this->outputBuffers.clear();
	}

	WebSocketSSLShard::WebSocketSSLShard(std::queue<DiscordCoreAPI::ConnectionPackage>* connectionsNew, int32_t currentBaseSocketAgentNew, int32_t currentShardNew,
		DiscordCoreAPI::ConfigManager* configManagerNew) noexcept {
		this->heartBeatStopWatch = DiscordCoreAPI::StopWatch<std::chrono::milliseconds>{ 10000ms };
		this->currentBaseSocketAgent = currentBaseSocketAgentNew;
		this->shard.push_back(currentShardNew);
		this->heartBeatStopWatch.resetTimer();
		this->shard.push_back(configManagerNew->getTotalShardCount());
		this->connections = connectionsNew;
		if (configManagerNew->getTextFormat() == DiscordCoreAPI::TextFormat::Etf) {
			this->dataOpCode = WebSocketOpCode::Op_Binary;
		} else {
			this->dataOpCode = WebSocketOpCode::Op_Text;
		}
	};

	void WebSocketSSLShard::processIO(std::unordered_map<SOCKET, std::unique_ptr<WebSocketSSLShard>>& theMap, int32_t waitTimeInms) {
		int32_t writeNfds{ 0 }, readNfds{ 0 }, finalNfds{ 0 };
		fd_set writeSet{}, readSet{};
		FD_ZERO(&writeSet);
		FD_ZERO(&readSet);
		bool didWeSetASocket{ false };
		for (auto& [key, value]: theMap) {
			if (value) {
				if (value->theSocket != SOCKET_ERROR) {
					if ((value->outputBuffers.size() > 0 || value->wantWrite) && !value->wantRead) {
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
					case SSL_ERROR_ZERO_RETURN: {
						value->disconnect();
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
					default: {
						ProcessingError theError{ reportSSLError("Shard [" + std::to_string(key) + "], in WebSocketSSLShard::processIO()::SSL_read_ex(), ", errorValue,
													  value->ssl) +
							reportError("Shard [" + std::to_string(key) + "], in WebSocketSSLShard::processIO()::SSL_read_ex(), ") };
						theError.theShard = value->shard[0];
						throw theError;
					}
				}
			}
			if (FD_ISSET(value->theSocket, &writeSet)) {
				value->wantRead = false;
				value->wantWrite = false;
				size_t writtenBytes{ 0 };
				std::string theString{};
				if (value->outputBuffers.size() > 0) {
					theString = std::move(value->outputBuffers.front());
					auto returnValue{ SSL_write_ex(value->ssl, theString.data(), theString.size(), &writtenBytes) };
					auto errorValue{ SSL_get_error(value->ssl, returnValue) };
					switch (errorValue) {
						case SSL_ERROR_NONE: {
							if (value->outputBuffers.size() > 0 && writtenBytes > 0) {
								value->outputBuffers.erase(value->outputBuffers.begin());
							}
							break;
						}
						case SSL_ERROR_ZERO_RETURN: {
							value->disconnect();
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
						default: {
							ProcessingError theError{ reportSSLError("Shard [" + std::to_string(key) + "], in WebSocketSSLShard::processIO()::SSL_write_ex(), ", errorValue,
														  value->ssl) +
								reportError("Shard [" + std::to_string(key) + "], in WebSocketSSLShard::processIO()::SSL_write_ex(), ") };
							theError.theShard = value->shard[0];
							throw theError;
						}
					}
				}
			}
		}
	}

	bool WebSocketSSLShard::writeData(const std::string& dataToWrite, bool priority) noexcept {
		std::string data = dataToWrite;
		if (priority && data.size() < (16 * 1024)) {
			size_t writtenBytes{ 0 };
			if (data.size() > 0 && this->ssl) {
				auto returnValue{ SSL_write_ex(this->ssl, data.data(), data.size(), &writtenBytes) };
				auto errorValue{ SSL_get_error(this->ssl, returnValue) };
				switch (errorValue) {
					case SSL_ERROR_NONE: {
						if (writtenBytes > 0) {
							data.clear();
							return true;
						}
						return false;
					}
					case SSL_ERROR_WANT_READ: {
						this->wantRead = true;
						return false;
					}
					case SSL_ERROR_WANT_WRITE: {
						this->wantWrite = true;
						return true;
					}
					default: {
						this->disconnect();
						return false;
					}
				}
			}
			return false;
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
					this->outputBuffers.push_back(newString);
					data.erase(data.begin(), data.begin() + amountToCollect);
					remainingBytes = data.size();
				}
			} else {
				this->outputBuffers.push_back(data);
			}
			return true;
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
		if (setsockopt(this->theSocket, SOL_SOCKET, SO_SNDBUF, static_cast<char*>(static_cast<void*>(&value)), sizeof(value))) {
			throw ConnectionError{ reportError("WebSocketSSLShard::connect()::setsockopt(), ") };
		}

		const char optionValue{ true };
		if (setsockopt(this->theSocket, IPPROTO_TCP, TCP_NODELAY, &optionValue, sizeof(int32_t))) {
			throw ConnectionError{ reportError("HttpsSSLClient::connect()::setsockopt(), ") };
		}

		linger optionValue02{};
		optionValue02.l_onoff = 0;
		if (setsockopt(this->theSocket, SOL_SOCKET, SO_LINGER, reinterpret_cast<char*>(&optionValue02), sizeof(linger))) {
			throw ConnectionError{ reportError("HttpsSSLClient::connect()::setsockopt(), ") };
		}

		if (::connect(this->theSocket, address->ai_addr, static_cast<int32_t>(address->ai_addrlen)) == SOCKET_ERROR) {
			throw ConnectionError{ reportError("WebSocketSSLShard::connect()::connect(), ") };
		}

		std::unique_lock<std::mutex> theLock{ SSLConnectionInterface::theMutex };
		if (this->ssl = SSL_new(SSLConnectionInterface::context); this->ssl == nullptr) {
			throw ConnectionError{ reportSSLError("WebSocketSSLShard::connect()::SSL_new(), ") };
		}
		theLock.unlock();

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

	std::string WebSocketSSLShard::getInputBuffer() noexcept {
		std::string theReturnString = this->inputBuffer;
		this->inputBuffer.clear();
		return theReturnString;
	}

	bool WebSocketSSLShard::areWeStillConnected() noexcept {
		if (this->theSocket != SOCKET_ERROR) {
			int32_t nfds{};
			fd_set errorfds{};
			FD_ZERO(&errorfds);
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

	int64_t WebSocketSSLShard::getBytesRead() noexcept {
		return this->bytesRead;
	}

	void WebSocketSSLShard::disconnect() noexcept {
		if (this->areWeConnected01.load()) {
			this->areWeConnected01.store(false);
			this->areWeConnected02.store(false);
			this->theSocket = SOCKET_ERROR;
			this->inputBuffer.clear();
			this->outputBuffers.clear();
			this->closeCode = static_cast<WebSocketCloseCode>(0);
			this->dataOpCode = WebSocketOpCode::Op_Binary;
			this->theState = WebSocketState::Connecting01;
			this->areWeHeartBeating = false;
			while (this->processedMessages.size() > 0) {
				this->processedMessages.pop();
			}
			if (this->connections) {
				DiscordCoreAPI::ConnectionPackage theData{};
				theData.voiceConnectionDataBufferMap = std::move(this->voiceConnectionDataBufferMap);
				theData.currentBaseSocketAgent = this->currentBaseSocketAgent;
				theData.currentShard = this->shard[0];
				this->connections->push(theData);
			}
		}
	}

	void DatagramSocketSSLClient::connect(const std::string& baseUrlNew, const std::string& portNew) {
#ifdef _WIN32
		this->theAddress.sin_addr.S_un.S_addr = inet_addr(baseUrlNew.c_str());
#else
		this->theAddress.sin_addr.s_addr = inet_addr(baseUrlNew.c_str());
#endif
		this->theAddress.sin_port = htons(stoi(portNew));
		this->theAddress.sin_family = AF_INET;

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
			throw ConnectionError{ reportError("DatagramSocketSSLClient::connect()::fcntl(), ") };
		}
#endif
	}

	void DatagramSocketSSLClient::writeData(std::string& dataToWrite) {
		if (dataToWrite.size() > static_cast<size_t>(16 * 1024)) {
			size_t remainingBytes{ dataToWrite.size() };
			while (remainingBytes > 0) {
				std::string newString{};
				size_t amountToCollect{};
				if (dataToWrite.size() >= static_cast<size_t>(1024 * 16)) {
					amountToCollect = static_cast<size_t>(1024 * 16);
				} else {
					amountToCollect = dataToWrite.size();
				}
				newString.insert(newString.begin(), dataToWrite.begin(), dataToWrite.begin() + amountToCollect);
				this->outputBuffers.push_back(newString);
				dataToWrite.erase(dataToWrite.begin(), dataToWrite.begin() + amountToCollect);
				remainingBytes = dataToWrite.size();
			}
		} else {
			this->outputBuffers.push_back(dataToWrite);
		}
	}

	std::string DatagramSocketSSLClient::getInputBuffer() noexcept {
		std::string theReturnString = this->inputBuffer;
		this->inputBuffer.clear();
		return theReturnString;
	}

	int64_t DatagramSocketSSLClient::getBytesRead() noexcept {
		return this->bytesRead;
	}

	void DatagramSocketSSLClient::processIO() {
		fd_set writeSet{}, readSet{};
		int32_t writeNfds{ 0 }, readNfds{ 0 }, finalNfds{ 0 };
		FD_ZERO(&writeSet);
		FD_ZERO(&readSet);
		FD_SET(this->theSocket, &readSet);
		if (this->outputBuffers.size() > 0) {
			FD_SET(this->theSocket, &writeSet);
			writeNfds = this->theSocket > writeNfds ? this->theSocket : writeNfds;
		}
		readNfds = this->theSocket > readNfds ? this->theSocket : readNfds;
		finalNfds = writeNfds > readNfds ? writeNfds : readNfds;

		timeval checkTime{ .tv_usec = 1000 };
		if (auto returnValue = select(finalNfds + 1, &readSet, &writeSet, nullptr, &checkTime); returnValue == SOCKET_ERROR) {
			throw ProcessingError{ reportError("DatagramSocketSSLClient::processIO()::select(), ") };
		} else if (returnValue == 0) {
			return;
		}

		if (FD_ISSET(this->theSocket, &readSet)) {
			std::string serverToClientBuffer{};
			serverToClientBuffer.resize(this->maxBufferSize);
			auto readBytes{ recv(this->theSocket, serverToClientBuffer.data(), static_cast<int32_t>(serverToClientBuffer.size()), 0) };
			if (readBytes > 0) {
				this->inputBuffer.insert(this->inputBuffer.end(), serverToClientBuffer.begin(), serverToClientBuffer.begin() + readBytes);
				this->bytesRead += readBytes;
			} else {
				throw ProcessingError{ reportError("DatagramSocketSSLClient::processIO()::recv(), ") };
			}
		}

		if (FD_ISSET(this->theSocket, &writeSet)) {
			if (this->outputBuffers.size() > 0) {
				std::string theString = this->outputBuffers.front();
				int32_t bytesToWrite{ static_cast<int32_t>(theString.size()) > this->maxBufferSize ? this->maxBufferSize : static_cast<int32_t>(theString.size()) };
				auto writtenBytes{ sendto(this->theSocket, theString.data(), bytesToWrite, 0, reinterpret_cast<sockaddr*>(&this->theAddress), sizeof(sockaddr)) };
				if (writtenBytes >= 0) {
					this->outputBuffers.erase(this->outputBuffers.begin());
					return;
				} else {
					throw ProcessingError{ reportError("DatagramSocketSSLClient::writeData()::send(), ") };
				}
			}
		}
	}

	SSL_CTXWrapper SSLConnectionInterface::context{ nullptr };
	std::mutex SSLConnectionInterface::theMutex{};
}