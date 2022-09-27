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

	void SOCKETWrapper::SOCKETDeleter::operator()(SOCKET* other) {
		if (*other != SOCKET_ERROR) {
#ifdef _WIN32
			shutdown(*other, SD_BOTH);
			closesocket(*other);
#else
			shutdown(*other, SHUT_RDWR);
			close(*other);
#endif
			*other = SOCKET_ERROR;
		};
	};

	SOCKETWrapper& SOCKETWrapper::operator=(SOCKETWrapper&& other) noexcept {
		if (*this->thePtr != SOCKET_ERROR) {
			this->thePtr.reset(new SOCKET{});
		}
		*this->thePtr = *other.thePtr;
		*other.thePtr = SOCKET_ERROR;
		return *this;
	}

	SOCKETWrapper::SOCKETWrapper(SOCKETWrapper&& other) noexcept {
		*this = std::move(other);
	}

	SOCKETWrapper& SOCKETWrapper::operator=(SOCKET other) noexcept {
		if (*this->thePtr != SOCKET_ERROR) {
			this->thePtr.reset(new SOCKET{});
		}
		*this->thePtr = other;
		return *this;
	}

	SOCKETWrapper::SOCKETWrapper(SOCKET other) noexcept {
		*this = other;
	}

	SOCKETWrapper::operator SOCKET*() noexcept {
		return this->thePtr.get();
	}

	SOCKETWrapper::operator SOCKET() noexcept {
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

	void RingBuffer::modifyReadOrWritePosition(RingBufferAccessType theType, size_t theSize) {
		if (theType == RingBufferAccessType::Read) {
			this->tail = (this->tail + theSize) % this->theArray.size();
			if (this->tail != this->head) {
				this->areWeFull = false;
			}
		} else {
			this->head = (this->head + theSize) % this->theArray.size();
			if (this->head == this->tail) {
				this->areWeFull = true;
			}
			if (this->head != this->tail) {
				this->areWeFull = false;
			}
		}
	}

	size_t RingBuffer::getUsedSpace() {
		if (this->areWeFull) {
			return this->theArray.size();
		}
		if ((this->head % this->theArray.size()) >= (this->tail % this->theArray.size())) {
			size_t freeSpace = this->theArray.size() - ((this->head % this->theArray.size()) - (this->tail % this->theArray.size()));
			return this->theArray.size() - freeSpace;
		} else {
			size_t freeSpace = (this->tail % this->theArray.size()) - (this->head % this->theArray.size());
			return this->theArray.size() - freeSpace;
		}
	}

	char* RingBuffer::getCurrentTail() {
		return (this->theArray.data() + (this->tail % (this->theArray.size())));
	}

	char* RingBuffer::getCurrentHead() {
		return (this->theArray.data() + (this->head % (this->theArray.size())));
	}

	bool RingBuffer::isItEmpty() {
		if (this->areWeFull) {
			return false;
		}
		return this->tail == this->head;
	}

	bool RingBuffer::isItFull() {
		return this->areWeFull;
	}

	void RingBuffer::clear() {
		this->areWeFull = false;
		this->tail = 0;
		this->head = 0;
	}

	void RingBufferArray::modifyReadOrWritePosition(RingBufferAccessType theType, size_t theSize) {
		if (theType == RingBufferAccessType::Read) {
			this->tail = (this->tail + theSize) % this->theArray.size();
			if (this->tail != this->head) {
				this->areWeFull = false;
			}
		} else {
			this->head = (this->head + theSize) % this->theArray.size();
			if (this->head == this->tail) {
				this->areWeFull = true;
			}
			if (this->head != this->tail) {
				this->areWeFull = false;
			}
		}
	}

	size_t RingBufferArray::getUsedSpace() {
		if (this->areWeFull) {
			return this->theArray.size();
		}
		if ((this->head % this->theArray.size()) >= (this->tail % this->theArray.size())) {
			size_t freeSpace = this->theArray.size() - ((this->head % this->theArray.size()) - (this->tail % this->theArray.size()));
			return this->theArray.size() - freeSpace;
		} else {
			size_t freeSpace = (this->tail % this->theArray.size()) - (this->head % this->theArray.size());
			return this->theArray.size() - freeSpace;
		}
	}

	size_t RingBufferArray ::getTotalSize() {
		size_t index01{};
		size_t index02{};
		size_t theSize{};
		if (this->tail > this->head) {
			index01 = this->head;
			index02 = this->tail;
		} else {
			index02 = this->head;
			index01 = this->tail;
		}
		for (uint32_t x = index01; x < index02; ++x) {
			theSize += this->theArray[x].getUsedSpace();
		}
		std::cout << "THE TOTAL SIZE: " << theSize << std::endl;
		return theSize;
	}

	std::string_view RingBufferArray::getStringView(size_t offSet, size_t size) {
		if (!this->getCurrentTail()->isItEmpty()) {
			size_t theSizeToCopy{};
			if (this->currentStringSize < this->currentStringUsed + size) {
				std::unique_ptr<char[]> theTempPtr{};
				theTempPtr.swap(this->referenceString);
				this->referenceString = std::make_unique<char[]>(this->currentStringSize + size);
				for (uint32_t x = 0; x < this->currentStringSize; ++x) {
					this->referenceString[x] = theTempPtr[x];
				}
			}
			while (size > 0) {
				if (size > 1024 * 16) {
					theSizeToCopy = this->getCurrentTail()->getUsedSpace();
				} else {
					theSizeToCopy = size;
				}
				std::cout << "THE OFFSET: " << offSet << "THE SIZE REAL: " << this->getCurrentTail()->getUsedSpace() << std::endl;
				this->currentStringUsed += size;
				memcpy(this->referenceString.get(), this->getCurrentTail()->getCurrentTail(), theSizeToCopy);
				this->getCurrentTail()->clear();
				this->modifyReadOrWritePosition(RingBufferAccessType::Read, 1);
				size -= theSizeToCopy;
			}
		}
		std::string_view theString{ this->referenceString.get() + offSet, this->currentStringUsed };
		std::cout << "THE STRING: " << theString << std::endl;
		std::cout << "THE OFFSET: " << offSet << "THE SIZE: " << size << std::endl;
		return theString;
	}

	void RingBufferArray::clearString() {
		this->currentStringUsed = 0;
	}

	RingBuffer* RingBufferArray::getCurrentTail() {
		return (this->theArray.data() + (this->tail % (this->theArray.size())));
	}

	RingBuffer* RingBufferArray::getCurrentHead() {
		return (this->theArray.data() + (this->head % (this->theArray.size())));
	}

	bool RingBufferArray::isItEmpty() {
		if (this->areWeFull) {
			return false;
		}
		return this->tail == this->head;
	}

	bool RingBufferArray::isItFull() {
		return this->areWeFull;
	}

	void RingBufferArray::clear() {
		this->areWeFull = false;
		this->tail = 0;
		this->head = 0;
	}

	SSLDataInterface::SSLDataInterface() noexcept {
	}

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

	SSLClient::SSLClient() noexcept {
	}

	bool SSLClient::connect(const std::string& baseUrl, const std::string& portNew, bool doWePrintErrorsNew, bool areWeAStandaloneSocketNew) noexcept {
		this->areWeAStandaloneSocket = areWeAStandaloneSocketNew;
		this->doWePrintErrorMessages = doWePrintErrorsNew;
		std::string addressString{};
		auto httpsFind = baseUrl.find("https://");
		auto comFind = baseUrl.find(".com");
		auto orgFind = baseUrl.find(".org");
		if (httpsFind != std::string::npos && comFind != std::string::npos) {
			addressString = baseUrl.substr(httpsFind + std::string("https://").size(), comFind + std::string(".com").size() - std::string("https://").size());
		} else if (httpsFind != std::string::npos && orgFind != std::string::npos) {
			addressString = baseUrl.substr(httpsFind + std::string("https://").size(), orgFind + std::string(".org").size() - std::string("https://").size());
		} else {
			addressString = baseUrl;
		}
		addrinfoWrapper hints{}, address{};
		hints->ai_family = AF_INET;
		hints->ai_socktype = SOCK_STREAM;
		hints->ai_protocol = IPPROTO_TCP;

		if (getaddrinfo(addressString.c_str(), portNew.c_str(), hints, address)) {
			if (this->doWePrintErrorMessages) {
				cout << reportError("SSLClient::connect::getaddrinfo(), to: " + baseUrl) << endl;
			}
			return false;
		}

		if (this->theSocket = socket(address->ai_family, address->ai_socktype, address->ai_protocol); this->theSocket == SOCKET_ERROR) {
			if (this->doWePrintErrorMessages) {
				cout << reportError("SSLClient::connect::socket(), to: " + baseUrl) << endl;
			}
			return false;
		}

		const char optionValue{ true };
		if (setsockopt(this->theSocket, IPPROTO_TCP, TCP_NODELAY, &optionValue, sizeof(int32_t))) {
			if (this->doWePrintErrorMessages) {
				cout << reportError("SSLClient::connect::setsockopt(), to: " + baseUrl) << endl;
			}
			return false;
		}

		if (setsockopt(this->theSocket, SOL_SOCKET, SO_KEEPALIVE, &optionValue, sizeof(int32_t))) {
			if (this->doWePrintErrorMessages) {
				cout << reportError("SSLClient::connect::setsockopt(), to: " + baseUrl) << endl;
			}
			return false;
		}

		if (::connect(this->theSocket, address->ai_addr, static_cast<int32_t>(address->ai_addrlen)) == SOCKET_ERROR) {
			if (this->doWePrintErrorMessages) {
				cout << reportError("SSLClient::connect::connect(), to: " + baseUrl) << endl;
			}
			return false;
		}

		std::unique_lock theLock{ SSLConnectionInterface::theMutex };
		if (this->ssl = SSL_new(this->context); this->ssl == nullptr) {
			if (this->doWePrintErrorMessages) {
				cout << reportSSLError("SSLClient::connect::SSL_new(), to: " + baseUrl) << endl;
			}
			return false;
		}
		theLock.unlock();

		if (auto theResult = SSL_set_fd(this->ssl, this->theSocket); theResult != 1) {
			if (this->doWePrintErrorMessages) {
				cout << reportSSLError("SSLClient::connect::SSL_set_fd(), to: " + baseUrl, theResult, this->ssl) << endl;
			}
			return false;
		}

		/* SNI */
		if (auto theResult = SSL_set_tlsext_host_name(this->ssl, addressString.c_str()); theResult != 1) {
			if (this->doWePrintErrorMessages) {
				cout << reportSSLError("SSLClient::connect::SSL_set_tlsext_host_name(), to: " + baseUrl, theResult, this->ssl) << endl;
			}
			return false;
		}

		if (auto theResult = SSL_connect(this->ssl); theResult != 1) {
			if (this->doWePrintErrorMessages) {
				cout << reportSSLError("SSLClient::connect::SSL_connect(), to: " + baseUrl, theResult, this->ssl) << endl;
			}
			return false;
		}

#ifdef _WIN32
		u_long value02{ 1 };
		if (auto returnValue = ioctlsocket(this->theSocket, FIONBIO, &value02); returnValue == SOCKET_ERROR) {
			if (this->doWePrintErrorMessages) {
				cout << reportError("SSLClient::connect::ioctlsocket(), to: " + baseUrl) << endl;
			}
			return false;
		}
#else
		if (auto returnValue = fcntl(this->theSocket, F_SETFL, fcntl(this->theSocket, F_GETFL, 0) | O_NONBLOCK); returnValue == SOCKET_ERROR) {
			return false;
		}
#endif
		return true;
	}

	std::vector<SSLClient*> SSLClient::processIO(std::vector<SSLClient*>& theVector) noexcept {
		std::vector<SSLClient*> theReturnValue{};
		PollFDWrapper readWriteSet{};
		for (uint32_t x = 0; x < theVector.size(); ++x) {
			pollfd theWrapper{};
			theWrapper.fd = theVector[x]->theSocket;
			if (!theVector[x]->outputBuffer.isItEmpty()) {
				theWrapper.events = POLLIN | POLLOUT;
			} else {
				theWrapper.events = POLLIN;
			}
			readWriteSet.theIndices.emplace_back(x);
			readWriteSet.thePolls.emplace_back(theWrapper);
		}

		if (readWriteSet.theIndices.size() == 0) {
			return theReturnValue;
		}

		if (auto returnValue = poll(readWriteSet.thePolls.data(), static_cast<unsigned long>(readWriteSet.theIndices.size()), 1); returnValue == SOCKET_ERROR) {
			for (uint32_t x = 0; x < readWriteSet.thePolls.size(); ++x) {
				if (readWriteSet.thePolls[x].revents & POLLERR || readWriteSet.thePolls[x].revents & POLLHUP || readWriteSet.thePolls[x].revents & POLLNVAL) {
					theReturnValue.emplace_back(theVector[readWriteSet.theIndices[x]]);
				}
			}
			return theReturnValue;

		} else if (returnValue == 0) {
			return theReturnValue;
		}

		for (uint32_t x = 0; x < readWriteSet.theIndices.size(); ++x) {
			if (readWriteSet.thePolls[x].revents & POLLOUT) {
				std::cout << "SSL CLIENT WHILE 233434" << std::endl;
				if (!theVector[readWriteSet.theIndices[x]]->processWriteData()) {
					theReturnValue.emplace_back(theVector[readWriteSet.theIndices[x]]);
					continue;
				}
			}
			if (readWriteSet.thePolls[x].revents & POLLIN) {
				//std::cout << "SSL CLIENT WHILE 453453" << std::endl;
				if (!theVector[readWriteSet.theIndices[x]]->processReadData()) {
					theReturnValue.emplace_back(theVector[readWriteSet.theIndices[x]]);
					continue;
				}
			}

			while (theVector[readWriteSet.theIndices[x]]->handleBuffer()) {
				std::cout << "SSL CLIENT WHILE 0101" << std::endl;
			}
		}
		return theReturnValue;
	}

	std::string_view SSLClient::getInputBuffer() noexcept {
		return this->inputBuffer.getStringView(0, this->inputBuffer.getTotalSize());
	}

	ProcessIOResult SSLClient::writeData(std::string& dataToWrite, bool priority) noexcept {
		if (dataToWrite.size() > 0 && this->ssl) {
			if (priority && dataToWrite.size() < static_cast<size_t>(16 * 1024)) {
				pollfd readWriteSet{};
				readWriteSet.fd = this->theSocket;
				readWriteSet.events = POLLOUT;
				if (auto returnValue = poll(&readWriteSet, 1, 1000); returnValue == SOCKET_ERROR) {
					return ProcessIOResult::Error;
				} else if (returnValue == 0) {
					return ProcessIOResult::Error;
				}

				if (readWriteSet.revents & POLLOUT) {
					memcpy(this->outputBuffer.getCurrentHead()->getCurrentHead(), dataToWrite.data(), dataToWrite.size());
					this->outputBuffer.getCurrentHead()->modifyReadOrWritePosition(RingBufferAccessType::Write, dataToWrite.size());
					this->outputBuffer.modifyReadOrWritePosition(RingBufferAccessType::Write, 1);
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
						memcpy(this->outputBuffer.getCurrentHead()->getCurrentHead(), newString.data(), newString.size());
						this->outputBuffer.getCurrentHead()->modifyReadOrWritePosition(RingBufferAccessType::Write, newString.size());
						this->outputBuffer.modifyReadOrWritePosition(RingBufferAccessType::Write, 1);
						dataToWrite.erase(dataToWrite.begin(), dataToWrite.begin() + amountToCollect);
						remainingBytes = dataToWrite.size();
						//std::cout << "SSL CLIENT WHILE 0202" << std::endl;
					}
				} else {
					memcpy(this->outputBuffer.getCurrentHead()->getCurrentHead(), dataToWrite.data(), dataToWrite.size());
					this->outputBuffer.getCurrentHead()->modifyReadOrWritePosition(RingBufferAccessType::Write, dataToWrite.size());
					this->outputBuffer.modifyReadOrWritePosition(RingBufferAccessType::Write, 1);
				}
			}
		}
		return ProcessIOResult::No_Error;
	}

	ProcessIOResult SSLClient::processIO(int32_t theWaitTimeInMs) noexcept {
		if (!this->areWeStillConnected()) {
			return ProcessIOResult::Error;
		}
		pollfd readWriteSet{};
		readWriteSet.fd = this->theSocket;
		if (this->outputBuffer.getUsedSpace() > 0) {
			//std::cout << "SSL CLIENT WHILE 787878" << std::endl;
			readWriteSet.events = POLLIN | POLLOUT;
		} else {
			std::cout << "SSL CLIENT WHILE 565656" << std::endl;
			readWriteSet.events = POLLIN;
		}
		ProcessIOResult theResult{ ProcessIOResult::No_Error };
		if (auto returnValue = poll(&readWriteSet, 1, theWaitTimeInMs); returnValue == SOCKET_ERROR) {
			if (this->doWePrintErrorMessages) {
				cout << reportSSLError("SSLClient::processIO()") << endl;
			}
			return ProcessIOResult::Error;
		} else if (returnValue == 0) {
			if (!this->areWeAStandaloneSocket) {
				while (this->handleBuffer()) {
					std::cout << "SSL CLIENT WHILE 0303" << std::endl;
				}
			}
			theResult = ProcessIOResult::No_Error;
		} else {
			if (readWriteSet.revents & POLLERR || readWriteSet.revents & POLLNVAL) {
				if (this->doWePrintErrorMessages) {
					cout << reportSSLError("SSLClient::processIO()") << endl;
				}
				return ProcessIOResult::Error;
			}
			if (readWriteSet.revents & POLLIN) {
				std::cout << "SSL CLIENT WHILE 454545" << std::endl;
				if (!this->processReadData()) {
					return ProcessIOResult::Error;
				}
			}
			if (readWriteSet.revents & POLLOUT) {
				std::cout << "SSL CLIENT WHILE 343434" << std::endl;
				if (!this->processWriteData()) {
					return ProcessIOResult::Error;
				}
			}
		}
		while (this->handleBuffer()) {
			std::cout << "SSL CLIENT WHILE 0404" << std::endl;
		}
		return theResult;
	}

	bool SSLClient::areWeStillConnected() noexcept {
		if (static_cast<SOCKET*>(this->theSocket) && this->theSocket != SOCKET_ERROR) {
			return true;
		} else {
			return false;
		}
	}

	bool SSLClient::processWriteData() noexcept {
		if (this->outputBuffer.getUsedSpace() > 0) {
			size_t bytesToWrite{};
			bytesToWrite = this->outputBuffer.getCurrentTail()->getUsedSpace();

			size_t writtenBytes{ 0 };
			auto returnValue{ SSL_write_ex(this->ssl, this->outputBuffer.getCurrentTail()->getCurrentTail(), bytesToWrite, &writtenBytes) };
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
						//std::cout << "WRITTEN BYTES: " << this->outputBuffer.getCurrentTail()->getCurrentTail() << std::endl;
						this->outputBuffer.getCurrentTail()->clear();
						this->outputBuffer.modifyReadOrWritePosition(RingBufferAccessType::Read, 1);
					}
					return true;
				}
				case SSL_ERROR_ZERO_RETURN: {
					return false;
				}
				default: {
					if (this->doWePrintErrorMessages) {
						cout << reportSSLError("SSLClient::processWriteData()", errorValue, this->ssl) << endl;
					}
					return false;
				}
			}
		}
		return true;
	}

	bool SSLClient::processReadData() noexcept {
		if (!this->inputBuffer.isItFull()) {
			do {
				size_t readBytes{ 0 };
				int64_t bytesToRead{ 1024 * 16 };
				auto returnValue{ SSL_read_ex(this->ssl, this->inputBuffer.getCurrentHead()->getCurrentHead(), bytesToRead, &readBytes) };
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
							this->inputBuffer.getCurrentHead()->modifyReadOrWritePosition(RingBufferAccessType::Write, readBytes);
							this->inputBuffer.modifyReadOrWritePosition(RingBufferAccessType::Write, 1);
							this->bytesRead += readBytes;
						}
						break;
					}
					case SSL_ERROR_ZERO_RETURN: {
						return false;
					}
					default: {
						if (this->doWePrintErrorMessages) {
							cout << reportSSLError("SSLClient::processReadData()", errorValue, this->ssl) << endl;
						}
						return false;
					}
				}
				//std::cout << "SSL CLIENT WHILE 0505: " << std::endl;
			} while (SSL_pending(this->ssl));
		}
		return true;
	}

	int64_t SSLClient::getBytesRead() noexcept {
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

	ProcessIOResult DatagramSocketClient::processIO(ProcessIOType theType) noexcept {
		ProcessIOResult theResult{ ProcessIOResult::No_Error };
		if (!this->areWeStillConnected() || !this->areWeStreamConnected) {
			return theResult;
		}
		pollfd readWriteSet{};
		readWriteSet.fd = this->theSocket;
		readWriteSet.events = POLLIN | POLLOUT;
		if (auto returnValue = poll(&readWriteSet, 1, 1000); returnValue == SOCKET_ERROR) {
			theResult = ProcessIOResult::Error;
			return theResult;
		} else if (returnValue == 0) {
			return theResult;
		} else {
			if (readWriteSet.revents & POLLRDNORM) {
				if (!this->processReadData()) {
					theResult = ProcessIOResult::Error;
				}
			}
			if (readWriteSet.revents & POLLWRNORM) {
				if (!this->processWriteData()) {
					theResult = ProcessIOResult::Error;
				}
			}
		}
		return theResult;
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
				this->outputBuffer.emplace_back(newString);
				dataToWrite.erase(dataToWrite.begin(), dataToWrite.begin() + amountToCollect);
				remainingBytes = dataToWrite.size();
			}
		} else {
			this->outputBuffer.emplace_back(dataToWrite);
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

	bool DatagramSocketClient::processWriteData() noexcept {
		if (this->outputBuffer.size() > 0 && this->areWeStreamConnected) {
			std::string clientToServerString = this->outputBuffer.front();
			int32_t writtenBytes = sendto(this->theSocket, clientToServerString.data(), static_cast<int32_t>(clientToServerString.size()), 0,
				( sockaddr* )&this->theStreamTargetAddress, sizeof(sockaddr));
			if (writtenBytes < 0) {
				return false;
			} else {
				this->outputBuffer.erase(this->outputBuffer.begin());
				return true;
			}
		}
		return true;
	}

	bool DatagramSocketClient::processReadData() noexcept {
		if (this->areWeStreamConnected) {
#ifdef _WIN32
			int32_t intSize = sizeof(this->theStreamTargetAddress);
#else
			socklen_t intSize = sizeof(this->theStreamTargetAddress);
#endif
			int32_t readBytes =
				recvfrom(this->theSocket, this->rawInputBuffer.data(), static_cast<int32_t>(this->maxBufferSize), 0, ( sockaddr* )&this->theStreamTargetAddress, &intSize);

			if (readBytes < 0) {
				return false;
			} else {
				this->inputBuffer.append(rawInputBuffer.data(), rawInputBuffer.data() + readBytes);
				this->bytesRead += readBytes;
				return true;
			}
		}
		return true;
	}

	int64_t DatagramSocketClient::getBytesRead() noexcept {
		return this->bytesRead;
	}

	void DatagramSocketClient::disconnect() noexcept {
		this->theSocket = SOCKET_ERROR;
		this->outputBuffer.clear();
		this->inputBuffer.clear();
	}

	DatagramSocketClient::~DatagramSocketClient() noexcept {
	}

	SSL_CTXWrapper SSLConnectionInterface::context{};
	std::mutex SSLConnectionInterface::theMutex{};

}