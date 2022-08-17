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

	std::string reportSSLError(const std::string& errorPosition, int32_t errorValue = 0, SSL* ssl = nullptr) noexcept {
		std::stringstream theStream{};
		theStream << DiscordCoreAPI::shiftToBrightRed() << errorPosition << " Error: ";
		if (ssl) {
			theStream << SSL_get_error(ssl, errorValue) << ", " << ERR_error_string(errorValue, nullptr) << DiscordCoreAPI::reset() << endl << endl;
		} else {
			theStream << ERR_error_string(errorValue, nullptr) << DiscordCoreAPI::reset() << endl << endl;
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
		theStream << WSAGetLastError() << ", " << string << DiscordCoreAPI::reset() << endl;
#else
		theStream << strerror(errno) << DiscordCoreAPI::reset();
#endif
		return theStream.str();
	}

	ConnectionError::ConnectionError(const std::string& theString) : std::runtime_error(theString){};

#ifdef _WIN32
	void WSADataWrapper::WSADataDeleter::operator()(WSADATA* other) {
		WSACleanup();
		delete other;
	}

	WSADataWrapper::WSADataWrapper() {
		auto returnValue = WSAStartup(MAKEWORD(2, 2), this->thePtr.get());
		if (returnValue) {

		}
	}
#endif

	void SSL_CTXWrapper::SSL_CTXDeleter::operator()(SSL_CTX* other) {
		if (other) {
			SSL_CTX_free(other);
			other = nullptr;
		}
	}

	SSL_CTXWrapper& SSL_CTXWrapper::operator=(SSL_CTX* other) {
		this->thePtr.reset(other);
		SSL_CTX_up_ref(other);
		return *this;
	}

	SSL_CTXWrapper::operator SSL_CTX*() {
		return this->thePtr.get();
	}

	void SSLWrapper::SSLDeleter::operator()(SSL* other) {
		if (other) {
			SSL_shutdown(other);
			SSL_free(other);
			other = nullptr;
		}
	}

	SSLWrapper& SSLWrapper::operator=(SSL* other) {
		this->thePtr.reset(other);
		SSL_up_ref(other);
		return *this;
	}

	SSLWrapper::operator SSL*() {
		return this->thePtr.get();
	}

	void SOCKETWrapper::SOCKETDeleter::operator()(SOCKET* other) {
		if (other) {
#ifdef _WIN32
			shutdown(*other, SD_BOTH);
			closesocket(*other);
#else
			shutdown(*other, SHUT_RDWR);
			close(*other);
#endif
			*other = SOCKET_ERROR;
			delete other;
		}
	}

	SOCKETWrapper& SOCKETWrapper::operator=(SOCKET other) {
		this->thePtr.reset(new SOCKET{ other });
		return *this;
	}

	SOCKETWrapper::operator SOCKET*() {
		return this->thePtr.get();
	}

	SOCKETWrapper::operator SOCKET() {
		return *this->thePtr;
	}
	
	sockaddr* sockaddrWrapper::operator->() {
		return reinterpret_cast<sockaddr*>(&this->thePtr);
	}

	sockaddrWrapper::operator sockaddr_in*() {
		return &this->thePtr;
	}

	sockaddrWrapper::operator sockaddr*() {
		return reinterpret_cast<sockaddr*>(&this->thePtr);
	}

	addrinfo* addrinfoWrapper::operator->() {
		return this->thePtr;
	}

	addrinfoWrapper::operator addrinfo**() {
		this->doWeClearAddrInfo = true;
		return &this->thePtr;
	}

	addrinfoWrapper::operator addrinfo*() {
		return this->thePtr;
	}

	addrinfoWrapper::~addrinfoWrapper() {
		if (this->doWeClearAddrInfo) {
			freeaddrinfo(this->thePtr);
		} else {
			delete this->thePtr;
		}
	}

	SSLConnectionInterface::~SSLConnectionInterface() noexcept {
		std::unique_lock theLock{ this->connectionMutex };
	}

	ProcessIOResult SSLClient::writeData(std::string& dataToWrite, bool priority) noexcept {
		std::unique_lock theLock{ this->connectionMutex };
		if (!this->areWeStillConnected()) {
			return ProcessIOResult::Disconnected;
		}
		if (dataToWrite.size() > 0 && this->ssl) {
			if (priority && dataToWrite.size() < static_cast<size_t>(16 * 1024)) {
				fd_set writeSet{};
				FD_ZERO(&writeSet);
				FD_SET(this->theSocket, &writeSet);

				timeval checkTime{ .tv_sec = 1, .tv_usec = 0 };
				if (auto returnValue = select(FD_SETSIZE + 1, nullptr, &writeSet, nullptr, &checkTime); returnValue == SOCKET_ERROR) {
					this->disconnect(true);
					return ProcessIOResult::Select_Failure;
				} else if (returnValue == 0) {
					return ProcessIOResult::Select_No_Return;
				}
				this->outputBuffers.emplace_back(dataToWrite);
				return this->writeDataProcess();
			} else {
				if (dataToWrite.size() > static_cast<size_t>(16 * 1024)) {
					size_t remainingBytes{ dataToWrite.size() };
					while (remainingBytes > 0) {
						std::cout << "WERE WRITING TCP SOCKET!" << std::endl;
						std::string newString{};
						size_t amountToCollect{};
						if (dataToWrite.size() >= static_cast<size_t>(1024 * 16)) {
							amountToCollect = static_cast<size_t>(1024 * 16);
						} else {
							amountToCollect = dataToWrite.size();
						}
						newString.insert(newString.begin(), dataToWrite.begin(), dataToWrite.begin() + amountToCollect);
						this->outputBuffers.emplace_back(newString);
						dataToWrite.erase(dataToWrite.begin(), dataToWrite.begin() + amountToCollect);
						remainingBytes = dataToWrite.size();
					}
				} else {
					this->outputBuffers.emplace_back(dataToWrite);
				}
				return ProcessIOResult::No_Error;
			}
		}
		return ProcessIOResult::Nothing_To_Write;
	}

	bool SSLClient::connect(const std::string& baseUrl, const std::string& portNew) noexcept {
		this->rawInputBuffer.resize(this->maxBufferSize);
		std::string stringNew{};
		auto httpsFind = baseUrl.find("https://");
		auto comFind = baseUrl.find(".com");
		auto orgFind = baseUrl.find(".org");
		if (httpsFind != std::string::npos && comFind != std::string::npos) {
			stringNew = baseUrl.substr(httpsFind + std::string("https://").size(), comFind + std::string(".com").size() - std::string("https://").size());
		} else if (httpsFind != std::string::npos && orgFind != std::string::npos) {
			stringNew = baseUrl.substr(httpsFind + std::string("https://").size(), orgFind + std::string(".org").size() - std::string("https://").size());
		} else {
			stringNew = baseUrl;
		}
		std::unique_lock theLock{ this->connectionMutex };
		addrinfoWrapper hints{}, address{};
		hints->ai_family = AF_INET;
		hints->ai_socktype = SOCK_STREAM;
		hints->ai_protocol = IPPROTO_TCP;
		
		if (this->context = SSL_CTX_new(TLS_client_method()); this->context == nullptr) {
			return false;
		}

		if (!SSL_CTX_set_min_proto_version(this->context, TLS1_2_VERSION)) {
			return false;
		}

		auto originalOptions{ SSL_CTX_get_options(this->context) | SSL_OP_IGNORE_UNEXPECTED_EOF };
		if (SSL_CTX_set_options(this->context, SSL_OP_IGNORE_UNEXPECTED_EOF) != originalOptions) {
			return false;
		}

		if (getaddrinfo(stringNew.c_str(), portNew.c_str(), hints, address)) {
			return false;
		}

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

		if (setsockopt(this->theSocket, SOL_SOCKET, SO_KEEPALIVE, &optionValue, sizeof(int32_t))) {
			return false;
		}

		linger optionValue02{};
		optionValue02.l_onoff = 0;
		if (setsockopt(this->theSocket, SOL_SOCKET, SO_LINGER, reinterpret_cast<char*>(&optionValue02), sizeof(linger))) {
			return false;
		}

		if (::connect(this->theSocket, address->ai_addr, static_cast<int32_t>(address->ai_addrlen)) == SOCKET_ERROR) {
			return false;
		}

		if (this->ssl = SSL_new(this->context); this->ssl == nullptr) {
			return false;
		}

		if (SSL_set_fd(this->ssl, this->theSocket) != 1) {
			return false;
		}

		/* SNI */
		if (SSL_set_tlsext_host_name(this->ssl, stringNew.c_str()) != 1) {
			return false;
		}

		if (SSL_connect(this->ssl) != 1) {
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

	ProcessIOResult SSLClient::processIO(int32_t theWaitTimeInms) noexcept {
		if (!this->areWeStillConnected()) {
			this->disconnect(true);
			return ProcessIOResult::Disconnected;
		}
		fd_set writeSet{}, readSet{};
		FD_ZERO(&writeSet);
		FD_ZERO(&readSet);

		std::unique_lock theLock{ this->connectionMutex };
		if ((this->outputBuffers.size() > 0 || this->wantWrite) && !this->wantRead) {
			FD_SET(this->theSocket, &writeSet);
		}
		FD_SET(this->theSocket, &readSet);

		timeval checkTime{ .tv_usec = theWaitTimeInms };
		ProcessIOResult returnValueReal{};
		if (auto returnValue = select(FD_SETSIZE + 1, &readSet, &writeSet, nullptr, &checkTime); returnValue == SOCKET_ERROR) {
			this->disconnect(true);
			returnValueReal = ProcessIOResult::Select_Failure;
		} else if (returnValue == 0) {
			returnValueReal = ProcessIOResult::Select_No_Return;
		}

		if (FD_ISSET(this->theSocket, &writeSet)) {
			returnValueReal = this->writeDataProcess();
		}
		if (FD_ISSET(this->theSocket, &readSet)) {
			returnValueReal = this->readDataProcess();
		}
		return returnValueReal;
	}

	std::string SSLClient::getInputBufferCopy() noexcept {
		std::unique_lock theLock{ this->connectionMutex };
		std::string theString = std::move(this->inputBuffer);
		this->inputBuffer.clear();
		return theString;
	}

	std::string& SSLClient::getInputBuffer() noexcept {
		return this->inputBuffer;
	}

	bool SSLClient::areWeStillConnected() noexcept {
		if (static_cast<SOCKET*>(this->theSocket) && this->theSocket != SOCKET_ERROR) {
			return true;
		} else {
			return false;
		}
	}

	ProcessIOResult SSLClient::writeDataProcess() noexcept {
		if (this->outputBuffers.size() > 0) {
			this->wantRead = false;
			this->wantWrite = false;
			size_t writtenBytes{ 0 };
			auto returnValue{ SSL_write_ex(this->ssl, this->outputBuffers.front().data(), this->outputBuffers.front().size(), &writtenBytes) };
			auto errorValue{ SSL_get_error(this->ssl, returnValue) };
			switch (errorValue) {
				case SSL_ERROR_NONE: {
					if (writtenBytes > 0) {
						this->outputBuffers.erase(this->outputBuffers.begin());
					}
					return ProcessIOResult::No_Error;
				}
				case SSL_ERROR_WANT_READ: {
					this->wantRead = true;
					return ProcessIOResult::No_Error;
				}
				case SSL_ERROR_WANT_WRITE: {
					this->wantWrite = true;
					return ProcessIOResult::No_Error;
				}
				case SSL_ERROR_ZERO_RETURN: {
					this->disconnect(true);
					return ProcessIOResult::SSL_Zero_Return;
				}
				default: {
					this->disconnect(true);
					return ProcessIOResult::SSL_Error;
				}
			}
		} else {
			return ProcessIOResult::Nothing_To_Write;
		}
	}

	ProcessIOResult SSLClient::readDataProcess() noexcept {
		this->wantRead = false;
		this->wantWrite = false;
		ProcessIOResult returnValueReal{};
		do {
			size_t readBytes{ 0 };
			auto returnValue{ SSL_read_ex(this->ssl, this->rawInputBuffer.data(), this->rawInputBuffer.size(), &readBytes) };
			auto errorValue{ SSL_get_error(this->ssl, returnValue) };
			switch (errorValue) {
				case SSL_ERROR_NONE: {
					if (readBytes > 0) {
						std::unique_lock theLock{ this->connectionMutex };
						this->inputBuffer.append(this->rawInputBuffer.data(), this->rawInputBuffer.data() + readBytes);
						this->bytesRead += readBytes;
					}
					returnValueReal = ProcessIOResult::No_Error;
					break;
				}
				case SSL_ERROR_WANT_READ: {
					this->wantRead = true;
					returnValueReal = ProcessIOResult::No_Error;
					break;
				}
				case SSL_ERROR_WANT_WRITE: {
					this->wantWrite = true;
					returnValueReal = ProcessIOResult::No_Error;
					break;
				}
				case SSL_ERROR_ZERO_RETURN: {
					this->disconnect(true);
					returnValueReal = ProcessIOResult::SSL_Zero_Return;
					break;
				}
				default: {
					this->disconnect(true);
					returnValueReal = ProcessIOResult::SSL_Error;
					break;
				}
			}
		} while (SSL_pending(this->ssl));
		return returnValueReal;
	}

	int64_t SSLClient::getBytesRead() noexcept {
		std::unique_lock theLock{ this->connectionMutex };
		return this->bytesRead;
	}

	DatagramSocketClient::DatagramSocketClient(DiscordCoreAPI::StreamType streamTypeNew) noexcept {
		this->streamType = streamTypeNew;
	}

	bool DatagramSocketClient::connect(const std::string& baseUrlNew, const std::string& portNew) noexcept {
		this->rawInputBuffer.resize(this->maxBufferSize);
		if (this->streamType == DiscordCoreAPI::StreamType::None || this->streamType == DiscordCoreAPI::StreamType::Client) {
			static_cast<sockaddr_in*>(this->theStreamTargetAddress)->sin_addr.s_addr = inet_addr(baseUrlNew.c_str());
			static_cast<sockaddr_in*>(this->theStreamTargetAddress)->sin_port = DiscordCoreAPI::reverseByteOrder(static_cast<unsigned short>(stoi(portNew)));
			static_cast<sockaddr_in*>(this->theStreamTargetAddress)->sin_family = AF_INET;
		} else {
			static_cast<sockaddr_in*>(this->theStreamTargetAddress)->sin_addr.s_addr = inet_addr(baseUrlNew.c_str());
			static_cast<sockaddr_in*>(this->theStreamTargetAddress)->sin_port = DiscordCoreAPI::reverseByteOrder(static_cast<unsigned short>(stoi(portNew)));
			static_cast<sockaddr_in*>(this->theStreamTargetAddress)->sin_family = AF_INET;
		}
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

		if (this->streamType == DiscordCoreAPI::StreamType::None || this->streamType == DiscordCoreAPI::StreamType::Client) {
			if (::connect(this->theSocket, address->ai_addr, static_cast<int32_t>(address->ai_addrlen))) {
				return false;
			}
		} else {
			DiscordCoreAPI::StopWatch theStopWatch{ 300s };
			while (!theStopWatch.hasTimePassed()) {
				std::string clientToServerString{};
				clientToServerString = "test string";

				if (auto theResult = bind(this->theSocket, this->theStreamTargetAddress, sizeof(sockaddr)); theResult != 0) {
					return false;
				}

				int32_t writtenBytes =
					sendto(this->theSocket, clientToServerString.data(), static_cast<int32_t>(clientToServerString.size()), 0, this->theStreamTargetAddress, sizeof(this->theStreamTargetAddress));
#ifdef _WIN32
				int32_t intSize = sizeof(this->theStreamTargetAddress);
#else
				socklen_t intSize = sizeof(this->theStreamTargetAddress);
#endif
				std::string serverToClientBuffer{};

				serverToClientBuffer.resize(11);
				int32_t readBytes = recvfrom(this->theSocket, serverToClientBuffer.data(), static_cast<int32_t>(serverToClientBuffer.size()), 0,
					reinterpret_cast<sockaddr*>(&this->theStreamTargetAddress), &intSize);
				if (readBytes >= 0) {
					break;
				}
			}
		}
		
	this->areWeStreamConnected = true;

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

	void DatagramSocketClient::processIO(ProcessIOType theType) noexcept {
		if (!this->areWeStillConnected() || !this->areWeStreamConnected) {
			return;
		}
		fd_set readSet{}, writeSet{};
		std::unique_lock theLock{ this->theMutex };
		switch (theType) {
			case ProcessIOType::Both: {
				FD_ZERO(&writeSet);
				FD_SET(this->theSocket, &writeSet);
			}
			case ProcessIOType::Read_Only: {
				FD_ZERO(&readSet);
				FD_SET(this->theSocket, &readSet);
				break;
			}
			case ProcessIOType::Write_Only: {
				FD_ZERO(&writeSet);
				FD_SET(this->theSocket, &writeSet);
				break;
			}
		}

		timeval checkTime{ .tv_sec = 0, .tv_usec = 500000 };
		if (auto returnValue = select(FD_SETSIZE, &readSet, &writeSet, nullptr, &checkTime); returnValue == SOCKET_ERROR) {
			this->disconnect();
			return;
		} else if (returnValue == 0) {
			return;
		} else {
			if (FD_ISSET(this->theSocket, &readSet)) {
				this->readDataProcess();
			}
			if (FD_ISSET(this->theSocket, &writeSet)) {
				this->writeDataProcess();
			}
		}
	}

	void DatagramSocketClient::writeData(std::string dataToWrite) noexcept {
		if (dataToWrite.size() > static_cast<size_t>(16 * 1024)) {
			size_t remainingBytes{ dataToWrite.size() };
			std::unique_lock theLock{ this->theMutex };
			while (remainingBytes > 0) {
				std::cout << "WERE WRITING DATAGRAM SOCKET!" << std::endl;
				std::string newString{};
				size_t amountToCollect{};
				if (dataToWrite.size() >= static_cast<size_t>(1024 * 16)) {
					amountToCollect = static_cast<size_t>(1024 * 16);
				} else {
					amountToCollect = dataToWrite.size();
				}
				newString.insert(newString.begin(), dataToWrite.begin(), dataToWrite.begin() + amountToCollect);
				this->outputBuffers.emplace_back(newString);
				dataToWrite.erase(dataToWrite.begin(), dataToWrite.begin() + amountToCollect);
				remainingBytes = dataToWrite.size();
			}
		} else {
			this->outputBuffers.emplace_back(dataToWrite);
		}
	}

	std::string& DatagramSocketClient::getInputBuffer() noexcept {
		return this->inputBuffer;
	}

	bool DatagramSocketClient::areWeStillConnected() noexcept {
		if (static_cast<SOCKET*>(this->theSocket) && this->theSocket != SOCKET_ERROR) {
			return true;
		} else {
			return false;
		}
	}

	void DatagramSocketClient::writeDataProcess() noexcept {
		if (this->outputBuffers.size() > 0 && this->areWeStreamConnected) {
			std::string clientToServerString = this->outputBuffers.front();
			int32_t writtenBytes =
				sendto(this->theSocket, clientToServerString.data(), static_cast<int32_t>(clientToServerString.size()), 0, this->theStreamTargetAddress, sizeof(sockaddr));
			if (writtenBytes < 0) {
				this->disconnect();
				return;
			} else {
				this->outputBuffers.erase(this->outputBuffers.begin());
			}
		}
	}

	void DatagramSocketClient::readDataProcess() noexcept {
		if (this->areWeStreamConnected) {
#ifdef _WIN32
			int32_t intSize = sizeof(this->theStreamTargetAddress);
#else
			socklen_t intSize = sizeof(this->theStreamTargetAddress);
#endif
			int32_t readBytes = recvfrom(this->theSocket, this->rawInputBuffer.data(), static_cast<int32_t>(rawInputBuffer.size()), 0, this->theStreamTargetAddress, &intSize);

			if (readBytes < 0) {
				this->disconnect();
				return;
			} else {
				this->inputBuffer.append(rawInputBuffer.data(), rawInputBuffer.data() + readBytes);
				this->bytesRead += readBytes;
			}
		}
	}

	int64_t DatagramSocketClient::getBytesRead() noexcept {
		std::unique_lock theLock{ this->theMutex };
		return this->bytesRead;
	}

	void DatagramSocketClient::disconnect() noexcept {
		std::unique_lock theLock{ this->theMutex };
		this->theSocket = SOCKET_ERROR;
		this->outputBuffers.clear();
		this->inputBuffer.clear();
	}

	DatagramSocketClient::~DatagramSocketClient() noexcept {
		std::unique_lock theLock{ this->theMutex };
	}

}