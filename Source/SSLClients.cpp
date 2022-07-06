/*
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

	ConnectionError::ConnectionError(const std::string& theString) : std::runtime_error(theString){};

#ifdef _WIN32
	void WSADataWrapper::WSADataDeleter::operator()(WSADATA* other) {
		WSACleanup();
		delete other;
	}

	WSADataWrapper::WSADataWrapper() {
		if (auto errorValue = WSAStartup(MAKEWORD(2, 2), this->thePtr.get()); errorValue != 0) {
			std::cout << DiscordCoreAPI::shiftToBrightRed() << "WSAStartup() Error: " << errorValue << ", ()";
			std::cout << DiscordCoreAPI::reset() << std::endl;
		}
	}
#endif

	void BIOWrapper::BIODeleter::operator()(BIO* other) {
		if (other) {
			BIO_free(other);
			other = nullptr;
		}
	}

	BIOWrapper& BIOWrapper::operator=(BIO * other) {
		this->bioPtr.reset(other);
		auto errorValue = BIO_up_ref(other);
		if (!errorValue) {
			std::cout << DiscordCoreAPI::shiftToBrightRed() << "BIO_up_ref() Error: " << ERR_error_string(errorValue, nullptr) << DiscordCoreAPI::reset() << std::endl;
		};
		return *this;
	}

	BIOWrapper::operator BIO*() {
		return this->bioPtr.get();
	}

	BIOWrapper::BIOWrapper(){};

	addrinfo * addrinfoWrapper::operator->() {
		if (this->addrinfoPtrTwo == nullptr) {
			throw ConnectionError{ "addrinfoWrapper::operator->(), addrinfoPtrTwo was nullptr." };
		}
		return this->addrinfoPtrTwo;
	}

	addrinfoWrapper::operator addrinfo**() {
		this->doWeClearAddrInfo = true;
		if (this->addrinfoPtrTwo == nullptr) {
			throw ConnectionError{ "addrinfoWrapper::addrinfo**(), addrinfoPtrTwo was nullptr." };
		}
		return &this->addrinfoPtrTwo;
	}

	addrinfoWrapper:: operator addrinfo*() {
		if (this->addrinfoPtrTwo == nullptr) {
			throw ConnectionError{ "addrinfoWrapper::addrinfo*(), addrinfoPtrTwo was nullptr." };
		}
		return this->addrinfoPtrTwo;
	}

	addrinfoWrapper::addrinfoWrapper(){};

	addrinfoWrapper::~addrinfoWrapper() {
		if (this->doWeClearAddrInfo) {
			freeaddrinfo(this->addrinfoPtrTwo);
		} else {
			delete this->addrinfoPtrTwo;
		}
	}

	void SSL_CTXWrapper::SSL_CTXDeleter::operator()(SSL_CTX* other) {
		if (other) {
			SSL_CTX_free(other);
			other = nullptr;
		}
	}

	SSL_CTXWrapper& SSL_CTXWrapper::operator=(SSL_CTX* other) {
		this->sslCTXPtr.reset(other);
		auto errorValue = SSL_CTX_up_ref(other);
		if (!errorValue) {
			std::cout << DiscordCoreAPI::shiftToBrightRed() << "SSL_CTX_up_ref() Error: " << ERR_error_string(errorValue, nullptr) << DiscordCoreAPI::reset() << std::endl;
		}
		return *this;
	}

	SSL_CTXWrapper::operator SSL_CTX*() {
		return this->sslCTXPtr.get();
	}

	SSL_CTXWrapper::SSL_CTXWrapper(){};

	void SSLWrapper::SSLDeleter::operator()(SSL* other) {
		if (other) {
			SSL_shutdown(other);
			SSL_free(other);
			other = nullptr;
		}
	}

	SSLWrapper& SSLWrapper::operator=(SSL * other) {
		this->sslPtr.reset(other);
		auto errorValue = SSL_up_ref(other);
		if (!errorValue) {
			std::cout << DiscordCoreAPI::shiftToBrightRed() << "SSL_up_ref() Error: " << ERR_error_string(errorValue, nullptr) << DiscordCoreAPI::reset() << std::endl;
		}
		return *this;
	}

	SSLWrapper::operator SSL*() {
		return this->sslPtr.get();
	}

	SSLWrapper::SSLWrapper(){};

	void SOCKETWrapper::SOCKETDeleter::operator()(std::atomic<SOCKET>* other) {
#ifdef _WIN32
		shutdown(other->load(), SD_BOTH);
		closesocket(other->load());
#else
		shutdown(other->load(), SHUT_RDWR);
		close(other->load());
#endif

		other->store(SOCKET_ERROR);
		delete other;
	}
	
	SOCKETWrapper& SOCKETWrapper::operator=(SOCKETWrapper && other) noexcept {
		if (this != &other) {
			this->socketPtr->store(other.socketPtr->load());
			other.socketPtr->store(SOCKET_ERROR);
		}
		return *this;
	}

	SOCKETWrapper:: SOCKETWrapper(SOCKETWrapper&& other) noexcept {
			*this = std::move(other);
	}

	SOCKETWrapper& SOCKETWrapper::operator=(SOCKET other) {
		this->socketPtr->store(other);
		return *this;
	}

	SOCKETWrapper::operator SOCKET() {
		return this->socketPtr->load();
	}

	SOCKETWrapper::SOCKETWrapper() {}


	std::string reportSSLError(const std::string& errorPosition, int32_t errorValue = 0, SSL* ssl = nullptr) noexcept {
		std::stringstream theStream{};
		if (ssl) {
			theStream << DiscordCoreAPI::shiftToBrightRed() << errorPosition << " Error: " << SSL_get_error(ssl, errorValue) << ", " << ERR_error_string(errorValue, nullptr)
					  << DiscordCoreAPI::reset() << std::endl
					  << std::endl;
		} else {
			theStream << DiscordCoreAPI::shiftToBrightRed() << errorPosition << " Error: " << DiscordCoreAPI::reset() << std::endl << std::endl;
		}

		return theStream.str();
	}

	std::string reportError(const std::string& errorPosition) noexcept {
		std::stringstream theStream{};
		theStream << DiscordCoreAPI::shiftToBrightRed() << errorPosition << " Error: ";
#ifdef _WIN32
		std::unique_ptr<char[]> string{ std::make_unique<char[]>(1024) };
	#ifdef UWP
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), ( LPWSTR )string.get(), 1024,
			NULL);
	#else
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), string.get(), 1024, NULL);
	#endif
		theStream << WSAGetLastError() << ", " << string << DiscordCoreAPI::reset() << std::endl;
#else
		theStream << strerror(errno) << DiscordCoreAPI::reset();
#endif
		return theStream.str();
	}

	void SSLConnectionInterface::initialize() {
		if (SSLConnectionInterface::context = SSL_CTX_new(TLS_client_method()); SSLConnectionInterface::context == nullptr) {
			throw ConnectionError{ reportSSLError("SSLConnectionInterface::initialize()::SSL_CTX_new()") };
		}

		if (!SSL_CTX_set_min_proto_version(SSLConnectionInterface::context, TLS1_2_VERSION)) {
			throw ConnectionError{ reportSSLError("SSLConnectionInterface::initialize()::SSL_CTX_set_min_proto_version()") };
		}

		auto originalOptions{ SSL_CTX_get_options(SSLConnectionInterface::context) | SSL_OP_IGNORE_UNEXPECTED_EOF };
		if (SSL_CTX_set_options(SSLConnectionInterface::context, SSL_OP_IGNORE_UNEXPECTED_EOF) != originalOptions) {
			throw ConnectionError{ reportSSLError("SSLConnectionInterface::initialize()::SSL_CTX_set_options()") };
		}
	}

	bool HttpsSSLClient::connect(const std::string& baseUrl, const std::string& portNew) noexcept {
		std::string stringNew{};
		if (baseUrl.find(".com") != std::string::npos) {
			stringNew =
				baseUrl.substr(baseUrl.find("https://") + std::string("https://").size(), baseUrl.find(".com") + std::string(".com").size() - std::string("https://").size());
		} else if (baseUrl.find(".org") != std::string::npos) {
			stringNew =
				baseUrl.substr(baseUrl.find("https://") + std::string("https://").size(), baseUrl.find(".org") + std::string(".org").size() - std::string("https://").size());
		}

		addrinfoWrapper hints{}, address{};
		hints->ai_family = AF_INET;
		hints->ai_socktype = SOCK_STREAM;
		hints->ai_protocol = IPPROTO_TCP;

		if (getaddrinfo(stringNew.c_str(), portNew.c_str(), hints, address)) {
			return false;
		}

		std::unique_lock theLock00{ this->theMutex01 };
		if (this->theSocket = socket(address->ai_family, address->ai_socktype, address->ai_protocol); this->theSocket == SOCKET_ERROR) {
			return false;
		}

		int32_t value{ this->maxBufferSize + 1 };
		if (setsockopt(this->theSocket, SOL_SOCKET, SO_SNDBUF, reinterpret_cast<char*>(&value), sizeof(value))) {
			return false;
		}

		const char optionValue{ true };
		if (setsockopt(this->theSocket, IPPROTO_TCP, TCP_NODELAY, &optionValue, sizeof(int32_t))) {
			return false;
		}

		linger optionValue02{};
		optionValue02.l_onoff = 0;
		if (setsockopt(this->theSocket, SOL_SOCKET, SO_LINGER, reinterpret_cast<char*>(&optionValue02), sizeof(linger))) {
			return false;
		}

		if (setsockopt(this->theSocket, SOL_SOCKET, SO_KEEPALIVE, &optionValue, sizeof(int32_t))) {
			return false;
		}

		if (::connect(this->theSocket, address->ai_addr, static_cast<int32_t>(address->ai_addrlen)) == SOCKET_ERROR) {
			return false;
		}

		std::unique_lock theLock01{ SSLConnectionInterface::theMutex };
		if (this->ssl = SSL_new(SSLConnectionInterface::context); this->ssl == nullptr) {
			return false;
		}
		theLock01.unlock();

		if (auto returnValue = SSL_set_fd(this->ssl, this->theSocket); !returnValue) {
			return false;
		}

		/* SNI */
		if (auto returnValue = SSL_set_tlsext_host_name(this->ssl, stringNew.c_str()); !returnValue) {
			return false;
		}

		if (auto returnValue = SSL_connect(this->ssl); !returnValue) {
			return false;
		}

#ifdef _WIN32
		u_long value02{ 1 };
		if (auto returnValue = ioctlsocket(this->theSocket, FIONBIO, &value02); returnValue == SOCKET_ERROR) {
			return false;
		}
#else
		if (auto returnValue = fcntl(this->theSocket, F_SETFL, fcntl(this->theSocket, F_GETFL, 0) | O_NONBLOCK); returnValue == SOCKET_ERROR) {
			return false;
		}
#endif
		this->theSSLState.store(SSLConnectionState::Connected);
		return true;
	}

	bool HttpsSSLClient::writeData(const std::string& dataToWrite, bool priority) noexcept {
		if (this->theSocket == SOCKET_ERROR) {
			return false;
		}
		std::string data = dataToWrite;
		if (data.size() > 0 && this->ssl) {
			if (priority && data.size() < static_cast<size_t>(16 * 1024)) {
				std::unique_lock theLock{ this->theMutex01 };
				fd_set writeSet{};
				int32_t writeNfds{ 0 };
				FD_ZERO(&writeSet);
				if (data.size() > 0) {
					FD_SET(this->theSocket, &writeSet);
					writeNfds = this->theSocket > writeNfds ? this->theSocket : writeNfds;
				} else {
					return false;
				}

				timeval checkTime{ .tv_usec = 1000 };
				if (auto returnValue = select(writeNfds + 1, nullptr, &writeSet, nullptr, &checkTime); returnValue == SOCKET_ERROR) {
					this->disconnect(true);
					return false;
				} else if (returnValue == 0) {
					return false;
				}

				if (FD_ISSET(this->theSocket, &writeSet)) {
					this->wantRead = false;
					this->wantWrite = false;
					size_t writtenBytes{ 0 };
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
						case SSL_ERROR_ZERO_RETURN: {
							this->disconnect(true);
							return false;
						}
						case SSL_ERROR_SSL: {
							this->disconnect(true);
							return false;
						}
						case SSL_ERROR_SYSCALL: {
							this->disconnect(true);
							return false;
						}
						case SSL_ERROR_WANT_READ: {
							this->wantRead = true;
							return false;
						}
						case SSL_ERROR_WANT_WRITE: {
							this->wantWrite = true;
							return false;
						}
						default: {
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
						std::unique_lock theLock{ this->theMutex01 };
						this->outputBuffers.push_back(newString);
						data.erase(data.begin(), data.begin() + amountToCollect);
						remainingBytes = data.size();
					}
				} else {
					std::unique_lock theLock{ this->theMutex01 };
					this->outputBuffers.push_back(data);
				}
				return true;
			}
		}
		return false;
	}

	void HttpsSSLClient::processIO(int32_t theWaitTimeInms) noexcept {
		if (this->theSocket == SOCKET_ERROR) {
			this->disconnect(true);
			return;
		}
		int32_t readNfds{ 0 }, writeNfds{ 0 }, finalNfds{ 0 };
		fd_set writeSet{}, readSet{};
		FD_ZERO(&writeSet);
		FD_ZERO(&readSet);
		std::unique_lock theLock{ this->theMutex01 };
		if ((this->outputBuffers.size() > 0 || this->wantWrite) && !this->wantRead) {
			FD_SET(this->theSocket, &writeSet);
			writeNfds = this->theSocket > writeNfds ? this->theSocket : writeNfds;
		}
		FD_SET(this->theSocket, &readSet);
		readNfds = this->theSocket > readNfds ? this->theSocket : readNfds;
		finalNfds = readNfds > writeNfds ? readNfds : writeNfds;

		timeval checkTime{ .tv_usec = theWaitTimeInms };
		if (auto returnValue = select(finalNfds + 1, &readSet, &writeSet, nullptr, &checkTime); returnValue == SOCKET_ERROR) {
			this->disconnect(true);
			return;
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
					this->disconnect(true);
					break;
				}
				case SSL_ERROR_SSL: {
					this->disconnect(true);
					break;
				}
				case SSL_ERROR_SYSCALL: {
					this->disconnect(true);
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
					break;
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
						this->disconnect(true);
						break;
					}
					case SSL_ERROR_SSL: {
						this->disconnect(true);
						break;
					}
					case SSL_ERROR_SYSCALL: {
						this->disconnect(true);
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
						break;
					}
				}
			}
		}
	}

	std::string HttpsSSLClient::getInputBuffer() noexcept {
		std::unique_lock theLock{ this->theMutex01 };
		std::string theReturnString = std::move(this->inputBuffer);
		this->inputBuffer.clear();
		return theReturnString;
	}

	bool HttpsSSLClient::areWeStillConnected() noexcept {
		if (this->theSocket == SOCKET_ERROR) {
			return false;
		} else {
			return true;
		}
	}

	int64_t HttpsSSLClient::getBytesRead() noexcept {
		std::unique_lock theLock{ this->theMutex01 };
		return this->bytesRead;
	}

	void HttpsSSLClient::disconnect(bool) noexcept {
		if (this->theSSLState.load() == SSLConnectionState::Connected) {
			std::unique_lock theLock{ this->theMutex01 };
			this->theSSLState.store(SSLConnectionState::Disconnected);
			this->theSocket = SOCKET_ERROR;
			this->inputBuffer.clear();
			this->outputBuffers.clear();
		}
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

	void WebSocketSSLShard::processIO(std::unordered_map<SOCKET, std::unique_ptr<WebSocketSSLShard>>& theMap, int32_t waitTimeInms) noexcept {
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
					}
					FD_SET(value->theSocket, &readSet);
					readNfds = value->theSocket > readNfds ? value->theSocket : readNfds;
					didWeSetASocket = true;
					finalNfds = readNfds > writeNfds ? readNfds : writeNfds;
				}
			}
		}

		if (!didWeSetASocket) {
			return;
		}

		timeval checkTime{ .tv_usec = waitTimeInms };
		if (auto returnValue = select(finalNfds + 1, &readSet, &writeSet, nullptr, &checkTime); returnValue == SOCKET_ERROR) {
			for (auto& [key, value]: theMap) {
				value->disconnect(true);
			}
			return;
		}

		for (auto& [key, value]: theMap) {
			if (FD_ISSET(value->theSocket, &readSet)) {
				std::unique_lock theLock{ value->theMutex01 };
				value->wantRead = false;
				value->wantWrite = false;
				std::string serverToClientBuffer{};
				serverToClientBuffer.resize(value->maxBufferSize);
				size_t readBytes{ 0 };
				auto returnValue{ SSL_read_ex(value->ssl, serverToClientBuffer.data(), serverToClientBuffer.size(), &readBytes) };
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
						value->disconnect(true);
						break;
					}
					case SSL_ERROR_SSL: {
						value->disconnect(true);
						break;
					}
					case SSL_ERROR_SYSCALL: {
						value->disconnect(true);
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
						break;
					}
				}
			}
			if (FD_ISSET(value->theSocket, &writeSet)) {
				std::unique_lock theLock{ value->theMutex01 };
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
							value->disconnect(true);
							break;
						}
						case SSL_ERROR_SSL: {
							value->disconnect(true);
							break;
						}
						case SSL_ERROR_SYSCALL: {
							value->disconnect(true);
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
							break;
						}
					}
				}
			}
		}
	}

	bool WebSocketSSLShard::connect(const std::string& baseUrlNew, const std::string& portNew) noexcept {
		addrinfoWrapper hints{}, address{};

		hints->ai_family = AF_INET;
		hints->ai_socktype = SOCK_STREAM;
		hints->ai_protocol = IPPROTO_TCP;

		if (getaddrinfo(baseUrlNew.c_str(), portNew.c_str(), hints, address)) {
			return false;
		}

		std::unique_lock theLock00{ this->theMutex01 };
		if (this->theSocket = socket(address->ai_family, address->ai_socktype, address->ai_protocol); this->theSocket == SOCKET_ERROR) {
			return false;
		}

		int32_t value{ this->maxBufferSize + 1 };
		if (setsockopt(this->theSocket, SOL_SOCKET, SO_SNDBUF, reinterpret_cast<char*>(&value), sizeof(value))) {
			return false;
		}

		const char optionValue{ true };
		if (setsockopt(this->theSocket, IPPROTO_TCP, TCP_NODELAY, &optionValue, sizeof(int32_t))) {
			return false;
		}

		linger optionValue02{};
		optionValue02.l_onoff = 0;
		if (setsockopt(this->theSocket, SOL_SOCKET, SO_LINGER, reinterpret_cast<char*>(&optionValue02), sizeof(linger))) {
			return false;
		}

		if (::connect(this->theSocket, address->ai_addr, static_cast<int32_t>(address->ai_addrlen))) {
			return false;
		}

		std::unique_lock theLock01{ SSLConnectionInterface::theMutex };
		if (this->ssl = SSL_new(SSLConnectionInterface::context); this->ssl == nullptr) {
			return false;
		}
		theLock01.unlock();

		if (auto returnValue = SSL_set_fd(this->ssl, this->theSocket); !returnValue) {
			return false;
		}

		/* SNI */
		if (auto returnValue = SSL_set_tlsext_host_name(this->ssl, baseUrlNew.c_str()); !returnValue) {
			return false;
		}

		if (auto returnValue = SSL_connect(this->ssl); !returnValue) {
			return false;
		}

#ifdef _WIN32
		u_long value02{ 1 };
		if (auto returnValue = ioctlsocket(this->theSocket, FIONBIO, &value02); returnValue == SOCKET_ERROR) {
			return false;
		}
#else
		if (auto returnValue = fcntl(this->theSocket, F_SETFL, fcntl(this->theSocket, F_GETFL, 0) | O_NONBLOCK); returnValue == SOCKET_ERROR) {
			return false;
		}
#endif
		this->theSSLState.store(SSLConnectionState::Connected);
		return true;
	}

	bool WebSocketSSLShard::writeData(const std::string& dataToWrite, bool priority) noexcept {
		if (this->theSocket == SOCKET_ERROR) {
			return false;
		}
		std::string data = dataToWrite;
		if (data.size() > 0 && this->ssl) {
			if (priority && data.size() < static_cast<size_t>(16 * 1024)) {
				std::unique_lock theLock{ this->theMutex01 };
				fd_set writeSet{};
				int32_t writeNfds{ 0 };
				FD_ZERO(&writeSet);
				if (data.size() > 0) {
					FD_SET(this->theSocket, &writeSet);
					writeNfds = this->theSocket > writeNfds ? this->theSocket : writeNfds;
				} else {
					return false;
				}

				timeval checkTime{ .tv_usec = 1000 };
				if (auto returnValue = select(writeNfds + 1, nullptr, &writeSet, nullptr, &checkTime); returnValue == SOCKET_ERROR) {
					this->disconnect(true);
					return false;
				} else if (returnValue == 0) {
					return false;
				}
				if (FD_ISSET(this->theSocket, &writeSet)) {
					this->wantRead = false;
					this->wantWrite = false;
					size_t writtenBytes{ 0 };
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
						case SSL_ERROR_ZERO_RETURN: {
							this->disconnect(true);
							return false;
						}
						case SSL_ERROR_SSL: {
							this->disconnect(true);
							return false;
						}
						case SSL_ERROR_SYSCALL: {
							this->disconnect(true);
							return false;
						}
						case SSL_ERROR_WANT_READ: {
							this->wantRead = true;
							return false;
						}
						case SSL_ERROR_WANT_WRITE: {
							this->wantWrite = true;
							return false;
						}
						default: {
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
						std::unique_lock theLock{ this->theMutex01 };
						this->outputBuffers.push_back(newString);
						data.erase(data.begin(), data.begin() + amountToCollect);
						remainingBytes = data.size();
					}
				} else {
					std::unique_lock theLock{ this->theMutex01 };
					this->outputBuffers.push_back(data);
				}
				return true;
			}
		}
		return false;
	}

	void WebSocketSSLShard::disconnect(bool doWeReconnect) noexcept {
		if (this->theSSLState.load() == SSLConnectionState::Connected) {
			std::unique_lock theLock{ this->theMutex01 };
			this->theSSLState.store(SSLConnectionState::Disconnected);
			this->theWebSocketState.store(WebSocketSSLShardState::Disconnected);
			this->theSocket = SOCKET_ERROR;
			this->inputBuffer.clear();
			this->outputBuffers.clear();
			this->closeCode = static_cast<WebSocketCloseCode>(0);
			this->areWeHeartBeating = false;
			while (this->processedMessages.size() > 0) {
				this->processedMessages.pop();
			}
			if (this->connections && doWeReconnect) {
				DiscordCoreAPI::ConnectionPackage theData{};
				theData.voiceConnectionDataBufferMap = std::move(this->voiceConnectionDataBufferMap);
				theData.currentBaseSocketAgent = this->currentBaseSocketAgent;
				theData.currentReconnectTries = this->currentReconnectTries;
				theData.currentShard = this->shard[0];
				this->connections->push(theData);
			}
		}
	}

	std::string WebSocketSSLShard::getInputBuffer() noexcept {
		std::unique_lock theLock{ this->theMutex01 };
		std::string theReturnString = std::move(this->inputBuffer);
		this->inputBuffer.clear();
		return theReturnString;
	}

	bool WebSocketSSLShard::areWeStillConnected() noexcept {
		if (this->theSocket != SOCKET_ERROR) {
			return true;
		} else {
			return false;
		}
	}

	int64_t WebSocketSSLShard::getBytesRead() noexcept {
		std::unique_lock theLock{ this->theMutex01 };
		return this->bytesRead;
	}

	bool DatagramSocketSSLClient::connect(const std::string& baseUrlNew, const std::string& portNew) noexcept {
		this->theAddress.sin_addr.s_addr = inet_addr(baseUrlNew.c_str());
		this->theAddress.sin_port = DiscordCoreAPI::reverseByteOrder(static_cast<unsigned short>(stoi(portNew)));
		this->theAddress.sin_family = AF_INET;

		addrinfoWrapper hints{}, address{};

		hints->ai_family = AF_INET;
		hints->ai_socktype = SOCK_DGRAM;
		hints->ai_protocol = IPPROTO_UDP;

		if (getaddrinfo(baseUrlNew.c_str(), portNew.c_str(), hints, address)) {
			return false;
		}

		if (this->theSocket = socket(address->ai_family, address->ai_socktype, address->ai_protocol); this->theSocket == SOCKET_ERROR) {
			return false;
		}

		if (::connect(this->theSocket, address->ai_addr, static_cast<int32_t>(address->ai_addrlen))) {
			return false;
		}

#ifdef _WIN32
		u_long value02{ 1 };
		if (ioctlsocket(this->theSocket, FIONBIO, &value02)) {
			return false;
		}
#else
		if (fcntl(this->theSocket, F_SETFL, fcntl(this->theSocket, F_GETFL, 0) | O_NONBLOCK)) {
			return false;
		}
#endif
		return true;
	}

	void DatagramSocketSSLClient::writeData(std::string& dataToWrite) noexcept {
		if (this->theSocket == SOCKET_ERROR) {
			this->disconnect();
			return;
		}
		if (dataToWrite.size() > 0) {
			auto writtenBytes{ sendto(this->theSocket, dataToWrite.data(), static_cast<int32_t>(dataToWrite.size()), 0, reinterpret_cast<sockaddr*>(&this->theAddress),
				sizeof(sockaddr)) };
			if (writtenBytes < 0) {
				this->disconnect();
				return;
			}
		}
	}

	std::string DatagramSocketSSLClient::getInputBuffer() noexcept {
		std::unique_lock theLock{ this->theMutex01 };
		std::string theReturnString = std::move(this->inputBuffer);
		this->inputBuffer.clear();
		return theReturnString;
	}

	bool DatagramSocketSSLClient::areWeStillConnected() noexcept {
		if (this->theSocket != SOCKET_ERROR) {
			return true;
		} else {
			return false;
		}
	}

	int64_t DatagramSocketSSLClient::getBytesRead() noexcept {
		std::unique_lock theLock{ this->theMutex01 };
		return this->bytesRead;
	}

	void DatagramSocketSSLClient::disconnect() noexcept {
		std::unique_lock theLock{ this->theMutex01 };
		this->theSocket = SOCKET_ERROR;
		this->inputBuffer.clear();
		this->outputBuffers.clear();
	}

	void DatagramSocketSSLClient::readData() noexcept {
		if (this->theSocket == SOCKET_ERROR) {
			return;
		}
		fd_set readSet{};
		int32_t readNfds{ 0 };
		std::unique_lock theLock{ this->theMutex01 };
		FD_ZERO(&readSet);
		FD_SET(this->theSocket, &readSet);
		readNfds = this->theSocket > readNfds ? this->theSocket : readNfds;

		timeval checkTime{ .tv_usec = 0 };
		if (auto returnValue = select(readNfds + 1, &readSet, nullptr, nullptr, &checkTime); returnValue == SOCKET_ERROR) {
			this->disconnect();
			return;
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
				this->disconnect();
				return;
			}
		}
	}

	SSL_CTXWrapper SSLConnectionInterface::context{};
	std::mutex SSLConnectionInterface::theMutex{};
}