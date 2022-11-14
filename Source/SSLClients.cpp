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
		std::stringstream stream{};
		stream << DiscordCoreAPI::shiftToBrightRed() << errorPosition << " Error: ";
		if (ssl) {
			stream << SSL_get_error(ssl, errorValue) << ", " << ERR_error_string(errorValue, nullptr) << DiscordCoreAPI::reset() << endl << endl;
		} else {
			stream << ERR_error_string(errorValue, nullptr) << DiscordCoreAPI::reset() << endl << endl;
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
	void WSADataWrapper::WSADataDeleter::operator()(WSADATA* other) {
		WSACleanup();
		delete other;
	}

	WSADataWrapper::WSADataWrapper() {
		auto returnValue = WSAStartup(MAKEWORD(2, 2), this->ptr.get());
		if (returnValue) {
			reportError("WSADataWrapper::WSADataWrapper()");
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
		this->ptr.reset(nullptr);
		this->ptr = std::unique_ptr<SSL_CTX, SSL_CTXDeleter>(other, SSL_CTXDeleter{});
		return *this;
	}

	SSL_CTXWrapper::operator SSL_CTX*() {
		return this->ptr.get();
	}

	void SSLWrapper::SSLDeleter::operator()(SSL* other) {
		if (other) {
			SSL_shutdown(other);
			SSL_free(other);
			other = nullptr;
		}
	}

	SSLWrapper& SSLWrapper::operator=(nullptr_t other) {
		this->ptr.reset(other);
		return *this;
	}

	SSLWrapper& SSLWrapper::operator=(SSL* other) {
		this->ptr.reset(nullptr);
		this->ptr = std::unique_ptr<SSL, SSLDeleter>(other, SSLDeleter{});
		return *this;
	}

	SSLWrapper::operator SSL*() {
		return this->ptr.get();
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
		this->ptr.reset(nullptr);
		this->ptr = std::unique_ptr<SOCKET, SOCKETDeleter>(new SOCKET{}, SOCKETDeleter{});
		*this->ptr = *other.ptr;
		*other.ptr = SOCKET_ERROR;
		return *this;
	}

	SOCKETWrapper::SOCKETWrapper(SOCKETWrapper&& other) noexcept {
		*this = std::move(other);
	}

	SOCKETWrapper& SOCKETWrapper::operator=(SOCKET other) noexcept {
		this->ptr.reset(nullptr);
		this->ptr = std::unique_ptr<SOCKET, SOCKETDeleter>(new SOCKET{}, SOCKETDeleter{});
		*this->ptr = other;
		return *this;
	}

	SOCKETWrapper::SOCKETWrapper(SOCKET other) noexcept {
		*this = other;
	}

	SOCKETWrapper::operator SOCKET*() noexcept {
		return this->ptr.get();
	}

	SOCKETWrapper::operator SOCKET() noexcept {
		return *this->ptr;
	}

	addrinfo* addrinfoWrapper::operator->() {
		return this->ptr;
	}

	addrinfoWrapper::operator addrinfo**() {
		return &this->ptr;
	}

	addrinfoWrapper::operator addrinfo*() {
		return this->ptr;
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

	bool SSLClient::connect(const std::string& baseUrl, const uint16_t portNew, bool doWePrintErrorsNew, bool areWeAStandaloneSocketNew) noexcept {
		this->areWeAStandaloneSocket = areWeAStandaloneSocketNew;
		this->doWePrintErrorMessages = doWePrintErrorsNew;
		std::string addressString{};
		auto httpsFind = baseUrl.find("https://");
		auto comFind = baseUrl.find(".com");
		auto orgFind = baseUrl.find(".org");
		if (httpsFind != std::string::npos && comFind != std::string::npos) {
			addressString =
				baseUrl.substr(httpsFind + std::string("https://").size(), comFind + std::string(".com").size() - std::string("https://").size());
		} else if (httpsFind != std::string::npos && orgFind != std::string::npos) {
			addressString =
				baseUrl.substr(httpsFind + std::string("https://").size(), orgFind + std::string(".org").size() - std::string("https://").size());
		} else {
			addressString = baseUrl;
		}
		addrinfoWrapper hints{}, address{};
		hints->ai_family = AF_INET;
		hints->ai_socktype = SOCK_STREAM;
		hints->ai_protocol = IPPROTO_TCP;

		if (getaddrinfo(addressString.c_str(), std::to_string(portNew).c_str(), hints, address)) {
			if (this->doWePrintErrorMessages) {
				cout << reportError("SSLClient::connect::getaddrinfo(), to: " + baseUrl) << endl;
			}
			return false;
		}

		if (this->socket = ::socket(address->ai_family, address->ai_socktype, address->ai_protocol); this->socket == SOCKET_ERROR) {
			if (this->doWePrintErrorMessages) {
				cout << reportError("SSLClient::connect::socket(), to: " + baseUrl) << endl;
			}
			return false;
		}

		const char optionValue{ true };
		if (setsockopt(this->socket, IPPROTO_TCP, TCP_NODELAY, &optionValue, sizeof(int32_t))) {
			if (this->doWePrintErrorMessages) {
				cout << reportError("SSLClient::connect::setsockopt(), to: " + baseUrl) << endl;
			}
			return false;
		}

		if (setsockopt(this->socket, SOL_SOCKET, SO_KEEPALIVE, &optionValue, sizeof(int32_t))) {
			if (this->doWePrintErrorMessages) {
				cout << reportError("SSLClient::connect::setsockopt(), to: " + baseUrl) << endl;
			}
			return false;
		}

		if (::connect(this->socket, address->ai_addr, static_cast<int32_t>(address->ai_addrlen)) == SOCKET_ERROR) {
			if (this->doWePrintErrorMessages) {
				cout << reportError("SSLClient::connect::connect(), to: " + baseUrl) << endl;
			}
			return false;
		}

		std::unique_lock lock{ SSLConnectionInterface::mutex };
		if (this->ssl = SSL_new(this->context); this->ssl == nullptr) {
			if (this->doWePrintErrorMessages) {
				cout << reportSSLError("SSLClient::connect::SSL_new(), to: " + baseUrl) << endl;
			}
			return false;
		}
		lock.unlock();

		if (auto result{ SSL_set_fd(this->ssl, this->socket) }; result != 1) {
			if (this->doWePrintErrorMessages) {
				cout << reportSSLError("SSLClient::connect::SSL_set_fd(), to: " + baseUrl, result, this->ssl) << endl;
			}
			return false;
		}

		/* SNI */
		if (auto result{ SSL_set_tlsext_host_name(this->ssl, addressString.c_str()) }; result != 1) {
			if (this->doWePrintErrorMessages) {
				cout << reportSSLError("SSLClient::connect::SSL_set_tlsext_host_name(), to: " + baseUrl, result, this->ssl) << endl;
			}
			return false;
		}

		if (auto result{ SSL_connect(this->ssl) }; result != 1) {
			if (this->doWePrintErrorMessages) {
				cout << reportSSLError("SSLClient::connect::SSL_connect(), to: " + baseUrl, result, this->ssl) << endl;
			}
			return false;
		}

#ifdef _WIN32
		u_long value02{ 1 };
		if (auto returnValue{ ioctlsocket(this->socket, FIONBIO, &value02) }; returnValue == SOCKET_ERROR) {
			if (this->doWePrintErrorMessages) {
				cout << reportError("SSLClient::connect::ioctlsocket(), to: " + baseUrl) << endl;
			}
			return false;
		}
#else
		if (auto returnValue{ fcntl(this->socket, F_SETFL, fcntl(this->socket, F_GETFL, 0) | O_NONBLOCK) }; returnValue == SOCKET_ERROR) {
			if (this->doWePrintErrorMessages) {
				cout << reportError("SSLClient::connect::ioctlsocket(), to: " + baseUrl) << endl;
			}
			return false;
		}
#endif
		return true;
	}

	std::vector<SSLClient*> SSLClient::processIO(std::unordered_map<uint32_t, std::unique_ptr<WebSocketSSLShard>>& shardMap) noexcept {
		std::vector<SSLClient*> returnValue{};
		PollFDWrapper readWriteSet{};
		for (auto& [key, value]: shardMap) {
			if (value && value->areWeStillConnected() && !value->areWeConnecting.load()) {
				pollfd fdSet{ .fd = static_cast<SOCKET>(value->socket) };
				if (value->outputBuffer.getUsedSpace() > 0) {
					fdSet.events = POLLIN | POLLOUT;
				} else {
					fdSet.events = POLLIN;
				}
				readWriteSet.indices.push_back(key);
				readWriteSet.polls.push_back(fdSet);
			}
		}

		if (readWriteSet.polls.size() == 0) {
			return returnValue;
		}

		if (auto returnValueNew = poll(readWriteSet.polls.data(), static_cast<unsigned long>(readWriteSet.polls.size()), 1);
			returnValueNew == SOCKET_ERROR) {
			for (size_t x = 0; x < readWriteSet.polls.size(); ++x) {
				if (readWriteSet.polls[x].revents & POLLERR || readWriteSet.polls[x].revents & POLLHUP || readWriteSet.polls[x].revents & POLLNVAL) {
					returnValue.emplace_back(shardMap[readWriteSet.indices[x]].get());
				}
			}
			return returnValue;

		} else if (returnValueNew == 0) {
			for (auto& [key, value]: shardMap) {
				if (value->areWeStillConnected() && !value->areWeConnecting.load() && !value->areWeAStandaloneSocket) {
					value->handleBuffer();
				}
			}
			return returnValue;
		}

		for (size_t x = 0; x < readWriteSet.polls.size(); ++x) {
			if (readWriteSet.polls[x].revents & POLLOUT) {
				if (!shardMap[readWriteSet.indices[x]]->processWriteData()) {
					returnValue.emplace_back(shardMap[readWriteSet.indices[x]].get());
					continue;
				}
			}
			if (readWriteSet.polls[x].revents & POLLIN) {
				if (!shardMap[readWriteSet.indices[x]]->processReadData()) {
					returnValue.emplace_back(shardMap[readWriteSet.indices[x]].get());
					continue;
				}
			}
		}
		for (auto& [key, value]: shardMap) {
			if (value->areWeStillConnected() && !value->areWeConnecting.load() && !value->areWeAStandaloneSocket) {
				value->handleBuffer();
			}
		}
		return returnValue;
	}

	ProcessIOResult SSLClient::writeData(std::string_view dataToWrite, bool priority) noexcept {
		if (this->areWeStillConnected()) {
			if (dataToWrite.size() > 0 && this->ssl) {
				if (priority && dataToWrite.size() < static_cast<uint64_t>(this->maxBufferSize)) {
					this->outputBuffer.clear();
					std::copy(dataToWrite.data(), dataToWrite.data() + dataToWrite.size(), this->outputBuffer.getCurrentHead()->getCurrentHead());
					this->outputBuffer.getCurrentHead()->modifyReadOrWritePosition(RingBufferAccessType::Write, dataToWrite.size());
					this->outputBuffer.modifyReadOrWritePosition(RingBufferAccessType::Write, 1);
					if (!this->processWriteData()) {
						return ProcessIOResult::Error;
					}
				} else {
					if (dataToWrite.size() > static_cast<uint64_t>(this->maxBufferSize)) {
						uint64_t remainingBytes{ dataToWrite.size() };
						uint64_t amountCollected{};
						while (remainingBytes > 0) {
							uint64_t amountToCollect{};
							if (dataToWrite.size() >= static_cast<uint64_t>(this->maxBufferSize)) {
								amountToCollect = static_cast<uint64_t>(this->maxBufferSize);
							} else {
								amountToCollect = dataToWrite.size();
							}
							std::copy(dataToWrite.data() + amountCollected, dataToWrite.data() + amountCollected + amountToCollect,
								this->outputBuffer.getCurrentHead()->getCurrentHead());
							this->outputBuffer.getCurrentHead()->modifyReadOrWritePosition(RingBufferAccessType::Write, amountToCollect);
							this->outputBuffer.modifyReadOrWritePosition(RingBufferAccessType::Write, 1);
							dataToWrite = std::basic_string_view{ dataToWrite.data() + amountToCollect, dataToWrite.size() - amountToCollect };
							remainingBytes = dataToWrite.size();
							amountCollected += amountToCollect;
						}
					} else {
						std::copy(dataToWrite.data(), dataToWrite.data() + dataToWrite.size(), this->outputBuffer.getCurrentHead()->getCurrentHead());
						this->outputBuffer.getCurrentHead()->modifyReadOrWritePosition(RingBufferAccessType::Write, dataToWrite.size());
						this->outputBuffer.modifyReadOrWritePosition(RingBufferAccessType::Write, 1);
					}
				}
			}
			return ProcessIOResult::No_Error;
		} else {
			return ProcessIOResult::Error;
		}
	}

	ProcessIOResult SSLClient::processIO(int32_t waitTimeInMs) noexcept {
		pollfd readWriteSet{ .fd = static_cast<SOCKET>(this->socket) };
		if (this->outputBuffer.getUsedSpace() > 0) {
			readWriteSet.events = POLLIN | POLLOUT;
		} else {
			readWriteSet.events = POLLIN;
		}
		if (auto returnValue = poll(&readWriteSet, 1, waitTimeInMs); returnValue == SOCKET_ERROR) {
			if (this->doWePrintErrorMessages) {
				cout << reportSSLError("SSLClient::processIO()") << endl;
			}
			return ProcessIOResult::Error;
		} else if (returnValue == 0) {
			if (!this->areWeAStandaloneSocket) {
				this->handleBuffer();
			}
			return ProcessIOResult::No_Error;
		} else {
			if (readWriteSet.revents & POLLERR || readWriteSet.revents & POLLNVAL) {
				if (this->doWePrintErrorMessages) {
					cout << reportSSLError("SSLClient::processIO()") << endl;
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
			this->handleBuffer();
		}
		return ProcessIOResult::No_Error;
	}

	std::basic_string_view<char8_t> SSLClient::getInputBuffer() noexcept {
		std::basic_string_view<char8_t> string{};
		if (this->inputBuffer.getUsedSpace() > 0 && this->inputBuffer.getCurrentTail()->getUsedSpace() > 0) {
			auto size = this->inputBuffer.getCurrentTail()->getUsedSpace();
			string = std::basic_string_view<char8_t>{ this->inputBuffer.getCurrentTail()->getCurrentTail(), size };
			this->inputBuffer.getCurrentTail()->clear();
			this->inputBuffer.modifyReadOrWritePosition(RingBufferAccessType::Read, 1);
		}
		return string;
	}

	bool SSLClient::areWeStillConnected() noexcept {
		if (static_cast<SOCKET*>(this->socket) && this->socket != SOCKET_ERROR) {
			return true;
		} else {
			return false;
		}
	}

	bool SSLClient::processWriteData() noexcept {
		if (this->outputBuffer.getUsedSpace() > 0) {
			uint64_t bytesToWrite{ this->outputBuffer.getCurrentTail()->getUsedSpace() };

			uint64_t writtenBytes{ 0 };
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
						this->outputBuffer.getCurrentTail()->clear();
						this->outputBuffer.modifyReadOrWritePosition(RingBufferAccessType::Read, 1);
					}
					return true;
				}
				case SSL_ERROR_ZERO_RETURN: {
					if (this->doWePrintErrorMessages) {
						cout << reportSSLError("SSLClient::processWriteData()", errorValue, this->ssl) << endl;
					}
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
				uint64_t readBytes{ 0 };
				uint64_t bytesToRead{};
				if (this->maxBufferSize > this->inputBuffer.getCurrentHead()->getFreeSpace()) {
					bytesToRead = this->inputBuffer.getCurrentHead()->getFreeSpace();
				} else {
					bytesToRead = this->maxBufferSize;
				}
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
						if (this->doWePrintErrorMessages) {
							cout << reportSSLError("SSLClient::processReadData()", errorValue, this->ssl) << endl;
						}
						return false;
					}
					default: {
						if (this->doWePrintErrorMessages) {
							cout << reportSSLError("SSLClient::processReadData()", errorValue, this->ssl) << endl;
						}
						return false;
					}
				}
			} while (SSL_pending(this->ssl));
		}

		if (!this->areWeAStandaloneSocket) {
			this->handleBuffer();
		}
		return true;
	}

	int64_t SSLClient::getBytesRead() noexcept {
		return this->bytesRead;
	}

	DatagramSocketClient::DatagramSocketClient(DiscordCoreAPI::StreamType streamTypeNew, bool doWePrintErrorsNew) noexcept {
		this->doWePrintErrors = doWePrintErrorsNew;
		this->streamTypeReal = streamTypeNew;
	}

	bool DatagramSocketClient::connect(const std::string& baseUrlNew, uint16_t portNew, bool haveWeGottenSignaled) noexcept {
		this->baseUrl = baseUrlNew;
		this->port = portNew;
		addrinfoWrapper hints{};
		hints->ai_family = AF_INET;
		hints->ai_socktype = SOCK_DGRAM;
		hints->ai_protocol = IPPROTO_UDP;

		if (getaddrinfo(baseUrlNew.c_str(), std::to_string(portNew).c_str(), hints, this->address)) {
			if (this->doWePrintErrors) {
				cout << reportError("DatagramSocketClient::connect::getaddrinfo(), to: " + baseUrlNew) << endl;
			}
			return false;
		}

		if (this->socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); this->socket == SOCKET_ERROR) {
			if (this->doWePrintErrors) {
				cout << reportError("DatagramSocketClient::connect::socket(), to: " + baseUrlNew) << endl;
			}
			return false;
		}

		std::unique_ptr<char> optVal{ new char{ 1 } };
		if (auto returnValue = setsockopt(this->socket, SOL_SOCKET, SO_REUSEADDR, optVal.get(), sizeof(optVal)); returnValue < 0) {
			if (this->doWePrintErrors) {
				cout << reportError("DatagramSocketClient::connect::setsockopt(), to: " + baseUrlNew) << endl;
			}
			return false;
		}

		if (this->streamTypeReal == DiscordCoreAPI::StreamType::None) {
			if (::connect(this->socket, this->address->ai_addr, static_cast<socklen_t>(this->address->ai_addrlen)) == SOCKET_ERROR) {
				if (this->doWePrintErrors) {
					cout << reportError("DatagramSocketClient::connect::connect(), to: " + baseUrlNew) << endl;
				}
				return false;
			}
		} else if (this->streamTypeReal == DiscordCoreAPI::StreamType::Client) {
			if (!haveWeGottenSignaled) {
				uint8_t chars[]{ "connecting" };
				std::u8string connectionString{ reinterpret_cast<char8_t*>(chars), std::size(chars) };
				int32_t result{};
				result = sendto(this->socket, reinterpret_cast<char*>(connectionString.data()), connectionString.size(), 0, this->address->ai_addr,
					this->address->ai_addrlen);
				result = recvfrom(this->socket, reinterpret_cast<char*>(connectionString.data()), connectionString.size(), 0, this->address->ai_addr,
					reinterpret_cast<socklen_t*>(&this->address->ai_addrlen));
			}
		} else {
			if (auto result = bind(this->socket, this->address->ai_addr, this->address->ai_addrlen); result != 0) {
				if (this->doWePrintErrors) {
					cout << reportError("DatagramSocketClient::connect::bind(), to: " + baseUrlNew) << endl;
				}
				return false;
			}
			if (!haveWeGottenSignaled) {
				std::string connectionString{};
				int32_t result{};
				connectionString.resize(10);
				result = recvfrom(this->socket, connectionString.data(), connectionString.size(), 0, this->address->ai_addr,
					reinterpret_cast<socklen_t*>(&this->address->ai_addrlen));
				connectionString = "connected1";
				result = 0;
				result = sendto(this->socket, connectionString.data(), connectionString.size(), 0, this->address->ai_addr, this->address->ai_addrlen);
			}
		}


#ifdef _WIN32
		u_long value02{ 1 };
		if (ioctlsocket(this->socket, FIONBIO, &value02)) {
			if (this->doWePrintErrors) {
				cout << reportError("DatagramSocketClient::connect::ioctlsocket(), to: " + baseUrlNew) << endl;
			}
			return false;
		}
#else
		if (fcntl(this->socket, F_SETFL, fcntl(this->socket, F_GETFL, 0) | O_NONBLOCK)) {
			if (this->doWePrintErrors) {
				cout << reportError("DatagramSocketClient::connect::ioctlsocket(), to: " + baseUrlNew) << endl;
			}
			return false;
		}
#endif

		return true;
	}

	ProcessIOResult DatagramSocketClient::processIO(ProcessIOType type) noexcept {
		pollfd readWriteSet{};
		readWriteSet.fd = this->socket;
		if (type == ProcessIOType::Both && this->outputBuffer.getUsedSpace() > 0) {
			readWriteSet.events = POLLIN | POLLOUT;
		} else if (type == ProcessIOType::Read_Only || this->outputBuffer.getUsedSpace() == 0) {
			readWriteSet.events = POLLIN;
		} else if (type == ProcessIOType::Write_Only && this->outputBuffer.getUsedSpace() > 0) {
			readWriteSet.events = POLLOUT;
		}

		ProcessIOResult result{ ProcessIOResult::No_Error };
		if (auto returnValue = poll(&readWriteSet, 1, 0); returnValue == SOCKET_ERROR) {
			if (this->doWePrintErrors) {
				cout << reportError("DatagramSocketClient::processIO()") << endl;
			}
			return ProcessIOResult::Error;
		} else if (returnValue == 0) {
			return ProcessIOResult::No_Error;
		} else {
			if (readWriteSet.revents & POLLIN) {
				if (this->streamTypeReal != DiscordCoreAPI::StreamType::None) {
				}
				if (!this->processReadData()) {
					result = ProcessIOResult::Error;
				} else {
					result = ProcessIOResult::No_Error;
				}
			}
			if (readWriteSet.revents & POLLOUT) {
				if (this->streamTypeReal != DiscordCoreAPI::StreamType::None) {
				}
				if (!this->processWriteData()) {
					result = ProcessIOResult::Error;
				} else {
					result = ProcessIOResult::No_Error;
				}
			}
		}
		return result;
	}

	void DatagramSocketClient::writeData(std::basic_string_view<char8_t> dataToWrite) noexcept {
		if (this->areWeStillConnected()) {
			if (dataToWrite.size() > static_cast<uint64_t>(this->maxBufferSize)) {
				uint64_t remainingBytes{ dataToWrite.size() };
				uint64_t amountCollected{};
				while (remainingBytes > 0) {
					uint64_t amountToCollect{};
					if (dataToWrite.size() >= static_cast<uint64_t>(this->maxBufferSize)) {
						amountToCollect = static_cast<uint64_t>(this->maxBufferSize);
					} else {
						amountToCollect = dataToWrite.size();
					}
					std::copy(dataToWrite.data() + amountCollected, dataToWrite.data() + amountCollected + amountToCollect,
						this->outputBuffer.getCurrentHead()->getCurrentHead());
					this->outputBuffer.getCurrentHead()->modifyReadOrWritePosition(RingBufferAccessType::Write, amountToCollect);
					this->outputBuffer.modifyReadOrWritePosition(RingBufferAccessType::Write, 1);
					dataToWrite = std::basic_string_view{ dataToWrite.data() + amountToCollect, dataToWrite.size() - amountToCollect };
					remainingBytes = dataToWrite.size();
					amountCollected += amountToCollect;
				}
			} else {
				std::copy(dataToWrite.data(), dataToWrite.data() + dataToWrite.size(), this->outputBuffer.getCurrentHead()->getCurrentHead());
				this->outputBuffer.getCurrentHead()->modifyReadOrWritePosition(RingBufferAccessType::Write, dataToWrite.size());
				this->outputBuffer.modifyReadOrWritePosition(RingBufferAccessType::Write, 1);
			}
		}
	}

	std::basic_string_view<char8_t> DatagramSocketClient::getInputBuffer() noexcept {
		std::basic_string_view<char8_t> string{};
		if (this->inputBuffer.getUsedSpace() > 0 && this->inputBuffer.getCurrentTail()->getUsedSpace() > 0) {
			auto size = this->inputBuffer.getCurrentTail()->getUsedSpace();
			string = std::basic_string_view<char8_t>{ this->inputBuffer.getCurrentTail()->getCurrentTail(), size };
			this->inputBuffer.getCurrentTail()->clear();
			this->inputBuffer.modifyReadOrWritePosition(RingBufferAccessType::Read, 1);
		}
		return string;
	}

	bool DatagramSocketClient::areWeStillConnected() noexcept {
		if (static_cast<SOCKET*>(this->socket) && this->socket != SOCKET_ERROR) {
			return true;
		} else {
			return false;
		}
	}

	bool DatagramSocketClient::processWriteData() noexcept {
		if (this->outputBuffer.getUsedSpace() > 0) {
			auto bytesToWrite{ this->outputBuffer.getCurrentTail()->getUsedSpace() };
			auto writtenBytes{ sendto(this->socket, reinterpret_cast<const char*>(this->outputBuffer.getCurrentTail()->getCurrentTail()),
				static_cast<int32_t>(bytesToWrite), 0, this->address->ai_addr, this->address->ai_addrlen) };
#ifdef _WIN32
			if (writtenBytes < 0 && WSAGetLastError() != WSAEWOULDBLOCK) {
#else
			if (writtenBytes < 0 && errno != EWOULDBLOCK) {
#endif
				return false;
			} else if (writtenBytes > 0) {
				this->outputBuffer.getCurrentTail()->clear();
				this->outputBuffer.modifyReadOrWritePosition(RingBufferAccessType::Read, 1);
				return true;
			}
		}
		return true;
	}

	bool DatagramSocketClient::processReadData() noexcept {
		bool returnValue{ true };
		int32_t readBytes{};
		do {
			if (!this->inputBuffer.isItFull()) {
				uint64_t bytesToRead{};
				if (this->maxBufferSize > this->inputBuffer.getCurrentHead()->getFreeSpace()) {
					bytesToRead = this->inputBuffer.getCurrentHead()->getFreeSpace();
				} else {
					bytesToRead = this->maxBufferSize;
				}

				readBytes = recvfrom(static_cast<SOCKET>(this->socket), reinterpret_cast<char*>(this->inputBuffer.getCurrentHead()->getCurrentHead()),
					static_cast<int32_t>(bytesToRead), 0, this->address->ai_addr, reinterpret_cast<socklen_t*>(&this->address->ai_addrlen));
#ifdef _WIN32
				if (readBytes < 0 && WSAGetLastError() != WSAEWOULDBLOCK) {
#else
				if (readBytes < 0 && errno != EWOULDBLOCK) {
#endif
					return true;
				} else if (readBytes > 0) {
					this->inputBuffer.getCurrentHead()->modifyReadOrWritePosition(RingBufferAccessType::Write, readBytes);
					this->inputBuffer.modifyReadOrWritePosition(RingBufferAccessType::Write, 1);
					this->bytesRead += readBytes;
					this->handleAudioBuffer();
					returnValue = true;
				}
			}
		} while (readBytes > 0);
		return returnValue;
	}

	int64_t DatagramSocketClient::sendUdpData(std::u8string& data) noexcept {
		return sendto(this->socket, reinterpret_cast<char*>(data.data()), data.size(), 0, this->address->ai_addr, this->address->ai_addrlen);
	}

	int64_t DatagramSocketClient::getBytesRead() noexcept {
		return this->bytesRead;
	}

	void DatagramSocketClient::disconnect() noexcept {
		this->socket = SOCKET_ERROR;
		this->outputBuffer.clear();
		this->inputBuffer.clear();
	}

	DatagramSocketClient ::~DatagramSocketClient() noexcept {
	}

	SSL_CTXWrapper SSLConnectionInterface::context{};
	std::mutex SSLConnectionInterface::mutex{};

}