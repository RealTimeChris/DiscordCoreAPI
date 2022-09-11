/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022 Chris M. (RealTimeChris)

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
/// SSLClents.cpp - Source file for the "SSL Client" stuff.
/// Dec 12, 2021
/// https://discordcoreapi.com
/// \file WebSocketSSLClients.cpp

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
		this->thePtr.reset(nullptr);
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
		this->thePtr.reset(nullptr);
		this->thePtr.reset(other);
		SSL_up_ref(other);
		return *this;
	}

	SSLWrapper::operator SSL*() {
		return this->thePtr.get();
	}

	void SOCKETWrapper::SOCKETDeleter::operator()(SOCKET other) {
		if (other != SOCKET_ERROR) {
#ifdef _WIN32
			shutdown(other, SD_BOTH);
			closesocket(other);
#else
			shutdown(other, SHUT_RDWR);
			close(other);
#endif
			other = SOCKET_ERROR;
		};
	};

	SOCKETWrapper& SOCKETWrapper::operator=(SOCKETWrapper&& other) noexcept {
		if (this->thePtr != SOCKET_ERROR) {
			SOCKETWrapper::SOCKETDeleter theDeleter{};
			theDeleter.operator()(this->thePtr);
		}
		this->thePtr = other.thePtr;
		other.thePtr = SOCKET_ERROR;
		return *this;
	}

	SOCKETWrapper::SOCKETWrapper(SOCKETWrapper&& other) noexcept {
		*this = std::move(other);
	}

	SOCKETWrapper& SOCKETWrapper::operator=(SOCKET other) noexcept {
		if (this->thePtr != SOCKET_ERROR) {
			SOCKETWrapper::SOCKETDeleter theDeleter{};
			theDeleter.operator()(this->thePtr);
		}
		this->thePtr = other;
		return *this;
	}

	SOCKETWrapper::SOCKETWrapper(SOCKET other) noexcept {
		*this = other;
	}

	SOCKETWrapper::operator SOCKET*() noexcept {
		return &this->thePtr;
	}

	SOCKETWrapper::operator SOCKET() noexcept {
		return static_cast<SOCKET>(this->thePtr);
	}

	SOCKETWrapper::~SOCKETWrapper() noexcept {
		if (this->thePtr != SOCKET_ERROR) {
			SOCKETWrapper::SOCKETDeleter theDeleter{};
			theDeleter.operator()(this->thePtr);
		}
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

	SSLDataInterface::SSLDataInterface() noexcept {}

	bool SSLConnectionInterface::initialize() noexcept {
		if (SSLConnectionInterface::context = SSL_CTX_new(TLS_client_method()); SSLConnectionInterface::context == nullptr) {
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

	std::string_view SSLDataInterface::getInputBuffer(uint32_t offSet, uint32_t length) noexcept {
		return std::string_view{};
	}

	std::string_view SSLDataInterface::getInputBuffer() noexcept {
		return std::string_view{};
	}

	WebSocketSSLClient::WebSocketSSLClient() noexcept {
		this->currentMessageBuffer.resize(1024 * 1024);
	}

	ConnectionResult WebSocketSSLClient::connect(const std::string& baseUrl, const std::string& portNew, bool doWePrintErrorsNew, bool areWeAStandaloneSocketNew) noexcept {
		this->areWeAStandaloneSocket = areWeAStandaloneSocketNew;
		this->doWePrintErrorMessages = doWePrintErrorsNew;
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
		addrinfoWrapper hints{}, address{};
		hints->ai_family = AF_INET;
		hints->ai_socktype = SOCK_STREAM;
		hints->ai_protocol = IPPROTO_TCP;

		if (getaddrinfo(stringNew.c_str(), portNew.c_str(), hints, address)) {
			if (this->doWePrintErrorMessages) {
				cout << reportError("WebSocketSSLClient::connect::getaddrinfo()") << endl;
			}
			return ConnectionResult::Error;
		}

		if (this->theSocket = socket(address->ai_family, address->ai_socktype, address->ai_protocol); this->theSocket == SOCKET_ERROR) {
			if (this->doWePrintErrorMessages) {
				cout << reportError("WebSocketSSLClient::connect::socket()") << endl;
			}
			return ConnectionResult::Error;
		}

		const char optionValue{ true };
		if (setsockopt(this->theSocket, IPPROTO_TCP, TCP_NODELAY, &optionValue, sizeof(int32_t))) {
			if (this->doWePrintErrorMessages) {
				cout << reportError("WebSocketSSLClient::connect::setsockopt()") << endl;
			}
			return ConnectionResult::Error;
		}

		if (setsockopt(this->theSocket, SOL_SOCKET, SO_KEEPALIVE, &optionValue, sizeof(int32_t))) {
			if (this->doWePrintErrorMessages) {
				cout << reportError("WebSocketSSLClient::connect::setsockopt()") << endl;
			}
			return ConnectionResult::Error;
		}

		if (::connect(this->theSocket, address->ai_addr, static_cast<int32_t>(address->ai_addrlen)) == SOCKET_ERROR) {
			if (this->doWePrintErrorMessages) {
				cout << reportError("WebSocketSSLClient::connect::connect()") << endl;
			}
			return ConnectionResult::Error;
		}

		std::unique_lock theLock{ SSLConnectionInterface::theMutex };
		if (this->ssl = SSL_new(this->context); this->ssl == nullptr) {
			if (this->doWePrintErrorMessages) {
				cout << reportSSLError("WebSocketSSLClient::connect::SSL_new()") << endl;
			}
			return ConnectionResult::Error;
		}
		theLock.unlock();

		if (auto theResult = SSL_set_fd(this->ssl, this->theSocket); theResult != 1) {
			if (this->doWePrintErrorMessages) {
				cout << reportSSLError("WebSocketSSLClient::connect::SSL_set_fd()", theResult, this->ssl) << endl;
			}
			return ConnectionResult::Error;
		}

		/* SNI */
		if (auto theResult = SSL_set_tlsext_host_name(this->ssl, stringNew.c_str()); theResult != 1) {
			if (this->doWePrintErrorMessages) {
				cout << reportSSLError("WebSocketSSLClient::connect::SSL_set_tlsext_host_name()", theResult, this->ssl) << endl;
			}
			return ConnectionResult::Error;
		}

		if (auto theResult = SSL_connect(this->ssl); theResult != 1) {
			if (this->doWePrintErrorMessages) {
				cout << reportSSLError("WebSocketSSLClient::connect::SSL_connect()", theResult, this->ssl) << endl;
			}
			return ConnectionResult::Error;
		}

#ifdef _WIN32
		u_long value02{ 1 };
		if (auto returnValue = ioctlsocket(this->theSocket, FIONBIO, &value02); returnValue == SOCKET_ERROR) {
			if (this->doWePrintErrorMessages) {
				cout << reportError("WebSocketSSLClient::connect::ioctlsocket()") << endl;
			}
			return ConnectionResult::Error;
		}
#else
		if (auto returnValue = fcntl(this->theSocket, F_SETFL, fcntl(this->theSocket, F_GETFL, 0) | O_NONBLOCK); returnValue == SOCKET_ERROR) {
			return ConnectionResult::Error;
		}
#endif
		this->currentState.store(DiscordCoreInternal::SSLShardState::Upgrading);
		return ConnectionResult::No_Error;
	}

	std::vector<WebSocketSSLClient*> WebSocketSSLClient::processIO(std::vector<WebSocketSSLClient*>& theVector) noexcept {
		std::vector<WebSocketSSLClient*> theReturnValue{};
		PollFDWrapper readWriteSet{};
		for (uint32_t x = 0; x < theVector.size(); ++x) {
			pollfd theWrapper{};
			theWrapper.fd = theVector[x]->theSocket;
			if (theVector[x]->outputBuffers.size() > 0) {
				theWrapper.events = POLLIN | POLLOUT;
			} else {
				theWrapper.events = POLLIN;
			}
			readWriteSet.theIndices.push_back(x);
			readWriteSet.thePolls.push_back(theWrapper);
		}

		if (readWriteSet.theIndices.size() == 0) {
			return theReturnValue;
		}

		if (auto returnValue = poll(readWriteSet.thePolls.data(), static_cast<unsigned long>(readWriteSet.theIndices.size()), 1); returnValue == SOCKET_ERROR) {
			for (uint32_t x = 0; x < readWriteSet.thePolls.size(); ++x) {
				if (readWriteSet.thePolls[x].revents & POLLERR || readWriteSet.thePolls[x].revents & POLLHUP || readWriteSet.thePolls[x].revents & POLLNVAL) {
					theReturnValue.push_back(theVector[readWriteSet.theIndices[x]]);
				}
			}
			return theReturnValue;

		} else if (returnValue == 0) {
			return theReturnValue;
		}

		for (uint32_t x = 0; x < readWriteSet.theIndices.size(); ++x) {
			if (readWriteSet.thePolls[x].revents & POLLOUT) {
				if (!theVector[readWriteSet.theIndices[x]]->processWriteData()) {
					theReturnValue.push_back(theVector[readWriteSet.theIndices[x]]);
					continue;
				}
			}
			if (readWriteSet.thePolls[x].revents & POLLIN) {
				if (!theVector[readWriteSet.theIndices[x]]->processReadData()) {
					theReturnValue.push_back(theVector[readWriteSet.theIndices[x]]);
					continue;
				}
			}

			while (theVector[readWriteSet.theIndices[x]]->handleBuffer(theVector[readWriteSet.theIndices[x]])) {
			}
		}
		return theReturnValue;
	}

	std::string_view WebSocketSSLClient::getInputBuffer(uint32_t offSet, uint32_t length) noexcept {
		std::string_view theString{ this->inputBuffer.getBufferPtr(offSet + length) + offSet, length };
		return theString;
	}

	ProcessIOResult WebSocketSSLClient::writeData(std::string& dataToWrite, bool priority) noexcept {
		if (dataToWrite.size() > 0 && this->ssl) {
			if (priority && dataToWrite.size() < static_cast<size_t>(16 * 1024)) {
				pollfd readWriteSet{};
				readWriteSet.fd = this->theSocket;
				readWriteSet.events = POLLOUT;
				if (auto returnValue = poll(&readWriteSet, 1, 1000); returnValue == SOCKET_ERROR) {
					this->disconnect(true);
					return ProcessIOResult::Error;
				} else if (returnValue == 0) {
					return ProcessIOResult::Error;
				}

				if (readWriteSet.revents & POLLOUT) {
					this->outputBuffers.emplace_back(dataToWrite);
					if (!this->processWriteData()) {
						return ProcessIOResult::Error;
					}
				}
			} else {
				if (dataToWrite.size() >= static_cast<size_t>(16 * 1024)) {
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
						this->outputBuffers.emplace_back(newString);
						dataToWrite.erase(dataToWrite.begin(), dataToWrite.begin() + amountToCollect);
						remainingBytes = dataToWrite.size();
					}
				} else {
					this->outputBuffers.emplace_back(dataToWrite);
				}
			}
		}
		return ProcessIOResult::No_Error;
	}

	ProcessIOResult WebSocketSSLClient::processIO(int32_t theWaitTimeInMs) noexcept {
		if (!this->areWeStillConnected()) {
			this->disconnect(true);
			return ProcessIOResult::Error;
		}
		pollfd readWriteSet{};
		readWriteSet.fd = this->theSocket;
		if (this->outputBuffers.size() > 0) {
			readWriteSet.events = POLLIN | POLLOUT;
		} else {
			readWriteSet.events = POLLIN;
		}

		if (auto returnValue = poll(&readWriteSet, 1, theWaitTimeInMs); returnValue == SOCKET_ERROR) {
			this->disconnect(true);
			if (this->doWePrintErrorMessages) {
				cout << reportError("WebSocketSSLClient::processIO()") << endl;
			}
			return ProcessIOResult::Error;
		} else if (returnValue == 0) {
			if (!this->areWeAStandaloneSocket) {
				while (this->handleBuffer(this)) {
				}
			}
			return ProcessIOResult::No_Error;
		} else {
			if (readWriteSet.revents & POLLERR || readWriteSet.revents & POLLHUP || readWriteSet.revents & POLLNVAL) {
				if (this->doWePrintErrorMessages) {
					cout << reportError("WebSocketSSLClient::processIO()") << endl;
				}
				return ProcessIOResult::Error;
			}
			if (readWriteSet.revents & POLLIN) {
				if (!this->processReadData()) {
					return ProcessIOResult::Error;
				}
			}
			if (readWriteSet.revents & POLLOUT) {
				if (!this->processWriteData()) {
					return ProcessIOResult::Error;
				}
			}
		}
		if (!this->areWeAStandaloneSocket) {
			while (this->handleBuffer(this)) {
			}
		}
		return ProcessIOResult::No_Error;
	}

	bool WebSocketSSLClient::areWeStillConnected() noexcept {
		if (static_cast<SOCKET*>(this->theSocket) && this->theSocket != SOCKET_ERROR) {
			return true;
		} else {
			return false;
		}
	}

	bool WebSocketSSLClient::processWriteData() noexcept {
		if (this->outputBuffers.size() > 0) {
			size_t writtenBytes{ 0 };
			auto returnValue{ SSL_write_ex(this->ssl, this->outputBuffers.front().data(), this->outputBuffers.front().size(), &writtenBytes) };
			auto errorValue{ SSL_get_error(this->ssl, returnValue) };
			switch (errorValue) {
				case SSL_ERROR_WANT_READ: {
					[[fallthrough]];
				}
				case SSL_ERROR_WANT_WRITE: {
					[[fallthrough]];
				}
				case SSL_ERROR_NONE: {
					if (writtenBytes > 0) {
						this->outputBuffers.erase(this->outputBuffers.begin());
					}
					return true;
				}
				case SSL_ERROR_ZERO_RETURN: {
					return false;
				}
				default: {
					if (this->doWePrintErrorMessages) {
						cout << reportSSLError("WebSocketSSLClient::processWriteData()", errorValue, this->ssl) << endl;
					}
					this->disconnect(true);
					return false;
				}
			}
		}
		return true;
	}

	bool WebSocketSSLClient::processReadData() noexcept {
		do {
			size_t readBytes{ 0 };
			size_t bytesToRead{};
			if (this->inputBuffer.getFreeSpace() < this->maxBufferSize) {
				bytesToRead = this->inputBuffer.getFreeSpace();
			} else {
				bytesToRead = this->maxBufferSize;
			}
			auto returnValue{ SSL_read_ex(this->ssl, this->rawInputBuffer.data(), bytesToRead, &readBytes) };
			auto errorValue{ SSL_get_error(this->ssl, returnValue) };
			switch (errorValue) {
				case SSL_ERROR_WANT_READ: {
					[[fallthrough]];
				}
				case SSL_ERROR_WANT_WRITE: {
					[[fallthrough]];
				}
				case SSL_ERROR_NONE: {
					if (readBytes > 0) {
						this->inputBuffer.writeData(this->rawInputBuffer.data(), readBytes);
						this->bytesRead += readBytes;
					}
					break;
				}
				case SSL_ERROR_ZERO_RETURN: {
					return false;
				}
				default: {
					if (this->doWePrintErrorMessages) {
						cout << reportSSLError("WebSocketSSLClient::processReadData()", errorValue, this->ssl) << endl;
					}
					this->disconnect(true);
					return false;
				}
			}
		} while (SSL_pending(this->ssl));
		return true;
	}

	int64_t WebSocketSSLClient::getBytesRead() noexcept {
		return this->bytesRead;
	}

	ConnectionResult HttpsSSLClient::connect(const std::string& baseUrl, const std::string& portNew, bool doWePrintErrorsNew, bool areWeAStandaloneSocketNew) noexcept {
		this->areWeAStandaloneSocket = areWeAStandaloneSocketNew;
		this->doWePrintErrorMessages = doWePrintErrorsNew;
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
		addrinfoWrapper hints{}, address{};
		hints->ai_family = AF_INET;
		hints->ai_socktype = SOCK_STREAM;
		hints->ai_protocol = IPPROTO_TCP;

		if (getaddrinfo(stringNew.c_str(), portNew.c_str(), hints, address)) {
			if (this->doWePrintErrorMessages) {
				cout << reportError("HttpsSSLClient::connect::getaddrinfo()") << endl;
			}
			return ConnectionResult::Error;
		}

		if (this->theSocket = socket(address->ai_family, address->ai_socktype, address->ai_protocol); this->theSocket == SOCKET_ERROR) {
			if (this->doWePrintErrorMessages) {
				cout << reportError("HttpsSSLClient::connect::socket()") << endl;
			}
			return ConnectionResult::Error;
		}

		const char optionValue{ true };
		if (setsockopt(this->theSocket, IPPROTO_TCP, TCP_NODELAY, &optionValue, sizeof(int32_t))) {
			if (this->doWePrintErrorMessages) {
				cout << reportError("HttpsSSLClient::connect::setsockopt()") << endl;
			}
			return ConnectionResult::Error;
		}

		if (setsockopt(this->theSocket, SOL_SOCKET, SO_KEEPALIVE, &optionValue, sizeof(int32_t))) {
			if (this->doWePrintErrorMessages) {
				cout << reportError("HttpsSSLClient::connect::setsockopt()") << endl;
			}
			return ConnectionResult::Error;
		}

		if (::connect(this->theSocket, address->ai_addr, static_cast<int32_t>(address->ai_addrlen)) == SOCKET_ERROR) {
			if (this->doWePrintErrorMessages) {
				cout << reportError("HttpsSSLClient::connect::connect()") << endl;
			}
			return ConnectionResult::Error;
		}

		std::unique_lock theLock{ SSLConnectionInterface::theMutex };
		if (this->ssl = SSL_new(this->context); this->ssl == nullptr) {
			if (this->doWePrintErrorMessages) {
				cout << reportSSLError("HttpsSSLClient::connect::SSL_new()") << endl;
			}
			return ConnectionResult::Error;
		}
		theLock.unlock();

		if (auto theResult = SSL_set_fd(this->ssl, this->theSocket); theResult != 1) {
			if (this->doWePrintErrorMessages) {
				cout << reportSSLError("HttpsSSLClient::connect::SSL_set_fd()", theResult, this->ssl) << endl;
			}
			return ConnectionResult::Error;
		}

		/* SNI */
		if (auto theResult = SSL_set_tlsext_host_name(this->ssl, stringNew.c_str()); theResult != 1) {
			if (this->doWePrintErrorMessages) {
				cout << reportSSLError("HttpsSSLClient::connect::SSL_set_tlsext_host_name()", theResult, this->ssl) << endl;
			}
			return ConnectionResult::Error;
		}

		if (auto theResult = SSL_connect(this->ssl); theResult != 1) {
			if (this->doWePrintErrorMessages) {
				cout << reportSSLError("HttpsSSLClient::connect::SSL_connect()", theResult, this->ssl) << endl;
			}
			return ConnectionResult::Error;
		}

#ifdef _WIN32
		u_long value02{ 1 };
		if (auto returnValue = ioctlsocket(this->theSocket, FIONBIO, &value02); returnValue == SOCKET_ERROR) {
			if (this->doWePrintErrorMessages) {
				cout << reportError("HttpsSSLClient::connect::ioctlsocket()") << endl;
			}
			return ConnectionResult::Error;
		}
#else
		if (auto returnValue = fcntl(this->theSocket, F_SETFL, fcntl(this->theSocket, F_GETFL, 0) | O_NONBLOCK); returnValue == SOCKET_ERROR) {
			return ConnectionResult::Error;
		}
#endif
		this->currentState.store(DiscordCoreInternal::SSLShardState::Upgrading);
		return ConnectionResult::No_Error;
	}

	std::vector<HttpsSSLClient*> HttpsSSLClient::processIO(std::vector<HttpsSSLClient*>& theVector) noexcept {
		std::vector<HttpsSSLClient*> theReturnValue{};
		PollFDWrapper readWriteSet{};
		for (uint32_t x = 0; x < theVector.size(); ++x) {
			pollfd theWrapper{};
			theWrapper.fd = theVector[x]->theSocket;
			if (theVector[x]->outputBuffers.size() > 0) {
				theWrapper.events = POLLIN | POLLOUT;
			} else {
				theWrapper.events = POLLIN;
			}
			readWriteSet.theIndices.push_back(x);
			readWriteSet.thePolls.push_back(theWrapper);
		}

		if (readWriteSet.theIndices.size() == 0) {
			return theReturnValue;
		}

		if (auto returnValue = poll(readWriteSet.thePolls.data(), static_cast<unsigned long>(readWriteSet.theIndices.size()), 1); returnValue == SOCKET_ERROR) {
			for (uint32_t x = 0; x < readWriteSet.thePolls.size(); ++x) {
				if (readWriteSet.thePolls[x].revents & POLLERR || readWriteSet.thePolls[x].revents & POLLHUP || readWriteSet.thePolls[x].revents & POLLNVAL) {
					theReturnValue.push_back(theVector[readWriteSet.theIndices[x]]);
				}
			}
			return theReturnValue;

		} else if (returnValue == 0) {
			return theReturnValue;
		}

		for (uint32_t x = 0; x < readWriteSet.theIndices.size(); ++x) {
			if (readWriteSet.thePolls[x].revents & POLLOUT) {
				if (!theVector[readWriteSet.theIndices[x]]->processWriteData()) {
					theReturnValue.push_back(theVector[readWriteSet.theIndices[x]]);
					continue;
				}
			}
			if (readWriteSet.thePolls[x].revents & POLLIN) {
				if (!theVector[readWriteSet.theIndices[x]]->processReadData()) {
					theReturnValue.push_back(theVector[readWriteSet.theIndices[x]]);
					continue;
				}
			}

			while (theVector[readWriteSet.theIndices[x]]->handleBuffer(theVector[readWriteSet.theIndices[x]])) {
			}
		}
		return theReturnValue;
	}

	ProcessIOResult HttpsSSLClient::writeData(std::string& dataToWrite, bool priority) noexcept {
		if (dataToWrite.size() > 0 && this->ssl) {
			if (priority && dataToWrite.size() < static_cast<size_t>(16 * 1024)) {
				pollfd readWriteSet{};
				readWriteSet.fd = this->theSocket;
				readWriteSet.events = POLLOUT;
				if (auto returnValue = poll(&readWriteSet, 1, 1000); returnValue == SOCKET_ERROR) {
					this->disconnect(true);
					return ProcessIOResult::Error;
				} else if (returnValue == 0) {
					return ProcessIOResult::Error;
				}

				if (readWriteSet.revents & POLLOUT) {
					this->outputBuffers.emplace_back(dataToWrite);
					if (!this->processWriteData()) {
						return ProcessIOResult::Error;
					}
				}
			} else {
				if (dataToWrite.size() >= static_cast<size_t>(16 * 1024)) {
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
						this->outputBuffers.emplace_back(newString);
						dataToWrite.erase(dataToWrite.begin(), dataToWrite.begin() + amountToCollect);
						remainingBytes = dataToWrite.size();
					}
				} else {
					this->outputBuffers.emplace_back(dataToWrite);
				}
			}
		}
		return ProcessIOResult::No_Error;
	}

	ProcessIOResult HttpsSSLClient::processIO(int32_t theWaitTimeInMs) noexcept {
		if (!this->areWeStillConnected()) {
			this->disconnect(true);
			return ProcessIOResult::Error;
		}
		pollfd readWriteSet{};
		readWriteSet.fd = this->theSocket;
		if (this->outputBuffers.size() > 0) {
			readWriteSet.events = POLLIN | POLLOUT;
		} else {
			readWriteSet.events = POLLIN;
		}

		if (auto returnValue = poll(&readWriteSet, 1, theWaitTimeInMs); returnValue == SOCKET_ERROR) {
			this->disconnect(true);
			if (this->doWePrintErrorMessages) {
				cout << reportError("HttpsSSLClient::processIO()") << endl;
			}
			return ProcessIOResult::Error;
		} else if (returnValue == 0) {
			if (!this->areWeAStandaloneSocket) {
				while (this->handleBuffer(this)) {
				}
			}
			return ProcessIOResult::No_Error;
		} else {
			if (readWriteSet.revents & POLLERR || readWriteSet.revents & POLLHUP || readWriteSet.revents & POLLNVAL) {
				if (this->doWePrintErrorMessages) {
					cout << reportError("HttpsSSLClient::processIO()") << endl;
				}
				return ProcessIOResult::Error;
			}
			if (readWriteSet.revents & POLLIN) {
				if (!this->processReadData()) {
					return ProcessIOResult::Error;
				}
			}
			if (readWriteSet.revents & POLLOUT) {
				if (!this->processWriteData()) {
					return ProcessIOResult::Error;
				}
			}
		}
		if (!this->areWeAStandaloneSocket) {
			while (this->handleBuffer(this)) {
			}
		}
		return ProcessIOResult::No_Error;
	}

	std::string HttpsSSLClient::getInputBufferRemove() noexcept {
		std::string theString{};
		theString.insert(theString.begin(), this->currentMessageBuffer.begin(), this->currentMessageBuffer.end());
		this->currentMessageBuffer.clear();
		return theString;
	}

	bool HttpsSSLClient::areWeStillConnected() noexcept {
		if (static_cast<SOCKET*>(this->theSocket) && this->theSocket != SOCKET_ERROR) {
			return true;
		} else {
			return false;
		}
	}

	bool HttpsSSLClient::processWriteData() noexcept {
		if (this->outputBuffers.size() > 0) {
			size_t writtenBytes{ 0 };
			auto returnValue{ SSL_write_ex(this->ssl, this->outputBuffers.front().data(), this->outputBuffers.front().size(), &writtenBytes) };
			auto errorValue{ SSL_get_error(this->ssl, returnValue) };
			switch (errorValue) {
				case SSL_ERROR_WANT_READ: {
					[[fallthrough]];
				}
				case SSL_ERROR_WANT_WRITE: {
					[[fallthrough]];
				}
				case SSL_ERROR_NONE: {
					if (writtenBytes > 0) {
						this->outputBuffers.erase(this->outputBuffers.begin());
					}
					return true;
				}
				case SSL_ERROR_ZERO_RETURN: {
					return false;
				}
				default: {
					if (this->doWePrintErrorMessages) {
						cout << reportSSLError("HttpsSSLClient::processWriteData()", errorValue, this->ssl) << endl;
					}
					this->disconnect(true);
					return false;
				}
			}
		}
		return true;
	}

	bool HttpsSSLClient::processReadData() noexcept {
		do {
			size_t readBytes{ 0 };
			auto returnValue{ SSL_read_ex(this->ssl, this->rawInputBuffer.data(), this->maxBufferSize, &readBytes) };
			auto errorValue{ SSL_get_error(this->ssl, returnValue) };
			switch (errorValue) {
				case SSL_ERROR_WANT_READ: {
					[[fallthrough]];
				}
				case SSL_ERROR_WANT_WRITE: {
					[[fallthrough]];
				}
				case SSL_ERROR_NONE: {
					if (readBytes > 0) {
						this->currentMessageBuffer.insert(this->currentMessageBuffer.end(), this->rawInputBuffer.begin(), this->rawInputBuffer.begin() + readBytes);
						this->bytesRead += readBytes;
					}
					break;
				}
				case SSL_ERROR_ZERO_RETURN: {
					return false;
				}
				default: {
					if (this->doWePrintErrorMessages) {
						cout << reportSSLError("HttpsSSLClient::processReadData()", errorValue, this->ssl) << endl;
					}
					this->disconnect(true);
					return false;
				}
			}
		} while (SSL_pending(this->ssl));
		return true;
	}

	int64_t HttpsSSLClient::getBytesRead() noexcept {
		return this->bytesRead;
	}

	DatagramSocketClient::DatagramSocketClient(DiscordCoreAPI::StreamType streamTypeNew) noexcept {
		this->streamType = streamTypeNew;
	}

	bool DatagramSocketClient::connect(const std::string& baseUrlNew, const std::string& portNew) noexcept {
		this->theStreamTargetAddress.sin_addr.s_addr = inet_addr(baseUrlNew.c_str());
		this->theStreamTargetAddress.sin_port = DiscordCoreAPI::reverseByteOrder<uint16_t>(static_cast<unsigned short>(stoi(portNew)));
		this->theStreamTargetAddress.sin_family = AF_INET;

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

				if (auto theResult = bind(this->theSocket, ( sockaddr* )&this->theStreamTargetAddress, sizeof(sockaddr)); theResult != 0) {
					return false;
				}

				int32_t writtenBytes = sendto(this->theSocket, clientToServerString.data(), static_cast<int32_t>(clientToServerString.size()), 0,
					( sockaddr* )&this->theStreamTargetAddress, sizeof(this->theStreamTargetAddress));
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
		pollfd readWriteSet{};
		readWriteSet.fd = this->theSocket;
		readWriteSet.events = POLLIN | POLLOUT;

		if (auto returnValue = poll(&readWriteSet, 1, 1000); returnValue == SOCKET_ERROR) {
			this->disconnect();
			return;
		} else if (returnValue == 0) {
			return;
		} else {
			if (readWriteSet.revents & POLLRDNORM) {
				this->processReadData();
			}
			if (readWriteSet.revents & POLLWRNORM) {
				this->processWriteData();
			}
		}
	}

	void DatagramSocketClient::writeData(std::string dataToWrite) noexcept {
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

	void DatagramSocketClient::processWriteData() noexcept {
		if (this->outputBuffers.size() > 0 && this->areWeStreamConnected) {
			std::string clientToServerString = this->outputBuffers.front();
			int32_t writtenBytes = sendto(this->theSocket, clientToServerString.data(), static_cast<int32_t>(clientToServerString.size()), 0,
				( sockaddr* )&this->theStreamTargetAddress, sizeof(sockaddr));
			if (writtenBytes < 0) {
				this->disconnect();
				return;
			} else {
				this->outputBuffers.erase(this->outputBuffers.begin());
			}
		}
	}

	void DatagramSocketClient::processReadData() noexcept {
		if (this->areWeStreamConnected) {
#ifdef _WIN32
			int32_t intSize = sizeof(this->theStreamTargetAddress);
#else
			socklen_t intSize = sizeof(this->theStreamTargetAddress);
#endif
			int32_t readBytes =
				recvfrom(this->theSocket, this->rawInputBuffer.data(), static_cast<int32_t>(this->maxBufferSize), 0, ( sockaddr* )&this->theStreamTargetAddress, &intSize);

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
		return this->bytesRead;
	}

	void DatagramSocketClient::disconnect() noexcept {
		this->theSocket = SOCKET_ERROR;
		this->outputBuffers.clear();
		this->inputBuffer.clear();
	}

	DatagramSocketClient::~DatagramSocketClient() noexcept {
	}

	SSL_CTXWrapper SSLConnectionInterface::context{};
	std::mutex SSLConnectionInterface::theMutex{};

}