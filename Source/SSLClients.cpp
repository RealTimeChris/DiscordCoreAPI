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
			stream << SSL_get_error(ssl, errorValue) << ", " << ERR_error_string(errorValue, nullptr) << DiscordCoreAPI::reset() << endl
				   << endl;
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
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, WSAGetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), ( LPWSTR )string, 1024, NULL);
	#else
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, WSAGetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), string, 1024, NULL);
	#endif
		stream << WSAGetLastError() << ", " << string << DiscordCoreAPI::reset() << endl;
#else
		stream << strerror(errno) << DiscordCoreAPI::reset();
#endif
		return stream.str();
	}

#ifdef _WIN32
	void WSADataWrapper::WSADataDeleter::operator()(WSADATA* other) noexcept {
		WSACleanup();
		delete other;
	}

	WSADataWrapper::WSADataWrapper() noexcept {
		auto returnValue = WSAStartup(MAKEWORD(2, 2), this->ptr.get());
		if (returnValue) {
			reportError("WSADataWrapper::WSADataWrapper()");
		}
	}
#endif

	void SSL_CTXWrapper::SSL_CTXDeleter::operator()(SSL_CTX* other) noexcept {
		if (other) {
			SSL_CTX_free(other);
			other = nullptr;
		}
	}

	SSL_CTXWrapper& SSL_CTXWrapper::operator=(SSL_CTX* other) noexcept {
		this->ptr.reset(nullptr);
		this->ptr = std::unique_ptr<SSL_CTX, SSL_CTXDeleter>(other, SSL_CTXDeleter{});
		return *this;
	}

	SSL_CTXWrapper::operator SSL_CTX*() noexcept {
		return this->ptr.get();
	}

	void SSLWrapper::SSLDeleter::operator()(SSL* other) noexcept {
		if (other) {
			SSL_shutdown(other);
			SSL_free(other);
			other = nullptr;
		}
	}

	SSLWrapper& SSLWrapper::operator=(nullptr_t other) noexcept {
		this->ptr.reset(other);
		return *this;
	}

	SSLWrapper& SSLWrapper::operator=(SSL* other) noexcept {
		this->ptr.reset(nullptr);
		this->ptr = std::unique_ptr<SSL, SSLDeleter>(other, SSLDeleter{});
		return *this;
	}

	SSLWrapper::operator SSL*() noexcept {
		return this->ptr.get();
	}

	void SOCKETWrapper::SOCKETDeleter::operator()(SOCKET* other) noexcept {
		if (*other != INVALID_SOCKET) {
			shutdown(*other, SHUT_RDWR);
			close(*other);
			*other = INVALID_SOCKET;
			delete other;
		};
	};

	SOCKETWrapper& SOCKETWrapper::operator=(SOCKET other) noexcept {
		this->ptr.reset(nullptr);
		this->ptr = std::unique_ptr<SOCKET, SOCKETDeleter>(std::make_unique<SOCKET>().release(), SOCKETDeleter{});
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

	addrinfo* addrinfoWrapper::operator->() noexcept {
		return this->ptr;
	}

	addrinfoWrapper::operator addrinfo**() noexcept {
		return &this->ptr;
	}

	addrinfoWrapper::operator addrinfo*() noexcept {
		return this->ptr;
	}

	bool SSLConnectionInterface::initialize() noexcept {
		if (SSLConnectionInterface::context = SSL_CTX_new(TLS_client_method()); !SSLConnectionInterface::context) {
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

	bool TCPSSLClient::connect(const std::string& baseUrl, const uint16_t portNew, bool doWePrintErrorsNew,
		bool areWeAStandaloneSocketNew) noexcept {
		this->areWeAStandaloneSocket = areWeAStandaloneSocketNew;
		this->doWePrintErrorMessages = doWePrintErrorsNew;
		std::string addressString{};
		auto httpsFind = baseUrl.find("https://");
		auto comFind = baseUrl.find(".com");
		auto orgFind = baseUrl.find(".org");
		if (httpsFind != std::string::npos && comFind != std::string::npos) {
			addressString = baseUrl.substr(httpsFind + std::string("https://").size(),
				comFind + std::string(".com").size() - std::string("https://").size());
		} else if (httpsFind != std::string::npos && orgFind != std::string::npos) {
			addressString = baseUrl.substr(httpsFind + std::string("https://").size(),
				orgFind + std::string(".org").size() - std::string("https://").size());
		} else {
			addressString = baseUrl;
		}
		addrinfoWrapper hints{}, address{};
		hints->ai_family = AF_INET;
		hints->ai_socktype = SOCK_STREAM;
		hints->ai_protocol = IPPROTO_TCP;

		if (getaddrinfo(addressString.c_str(), std::to_string(portNew).c_str(), hints, address)) {
			if (this->doWePrintErrorMessages) {
				cout << reportError("TCPSSLClient::connect::getaddrinfo(), to: " + baseUrl) << endl;
			}
			return false;
		}

		if (this->socket = ::socket(address->ai_family, address->ai_socktype, address->ai_protocol); this->socket == INVALID_SOCKET) {
			if (this->doWePrintErrorMessages) {
				cout << reportError("TCPSSLClient::connect::socket(), to: " + baseUrl) << endl;
			}
			return false;
		}

		const char optionValue{ true };
		if (setsockopt(this->socket, IPPROTO_TCP, TCP_NODELAY, &optionValue, sizeof(int32_t))) {
			if (this->doWePrintErrorMessages) {
				cout << reportError("TCPSSLClient::connect::setsockopt(), to: " + baseUrl) << endl;
			}
			return false;
		}

		if (setsockopt(this->socket, SOL_SOCKET, SO_KEEPALIVE, &optionValue, sizeof(int32_t))) {
			if (this->doWePrintErrorMessages) {
				cout << reportError("TCPSSLClient::connect::setsockopt(), to: " + baseUrl) << endl;
			}
			return false;
		}

		if (::connect(this->socket, address->ai_addr, static_cast<int32_t>(address->ai_addrlen)) == SOCKET_ERROR) {
			if (this->doWePrintErrorMessages) {
				cout << reportError("TCPSSLClient::connect::connect(), to: " + baseUrl) << endl;
			}
			return false;
		}

		std::unique_lock lock{ SSLConnectionInterface::mutex };
		if (this->ssl = SSL_new(this->context); !this->ssl) {
			if (this->doWePrintErrorMessages) {
				cout << reportSSLError("TCPSSLClient::connect::SSL_new(), to: " + baseUrl) << endl;
			}
			return false;
		}
		lock.unlock();

		if (auto result{ SSL_set_fd(this->ssl, this->socket) }; result != 1) {
			if (this->doWePrintErrorMessages) {
				cout << reportSSLError("TCPSSLClient::connect::SSL_set_fd(), to: " + baseUrl, result, this->ssl) << endl;
			}
			return false;
		}

		/* SNI */
		if (auto result{ SSL_set_tlsext_host_name(this->ssl, addressString.c_str()) }; result != 1) {
			if (this->doWePrintErrorMessages) {
				cout << reportSSLError("TCPSSLClient::connect::SSL_set_tlsext_host_name(), to: " + baseUrl, result, this->ssl) << endl;
			}
			return false;
		}

		if (auto result{ SSL_connect(this->ssl) }; result != 1) {
			if (this->doWePrintErrorMessages) {
				cout << reportSSLError("TCPSSLClient::connect::SSL_connect(), to: " + baseUrl, result, this->ssl) << endl;
			}
			return false;
		}

#ifdef _WIN32
		u_long value02{ 1 };
		if (auto returnValue{ ioctlsocket(this->socket, FIONBIO, &value02) }; returnValue == SOCKET_ERROR) {
			if (this->doWePrintErrorMessages) {
				cout << reportError("TCPSSLClient::connect::ioctlsocket(), to: " + baseUrl) << endl;
			}
			return false;
		}
#else
		if (auto returnValue{ fcntl(this->socket, F_SETFL, fcntl(this->socket, F_GETFL, 0) | O_NONBLOCK) }; returnValue == SOCKET_ERROR) {
			if (this->doWePrintErrorMessages) {
				cout << reportError("TCPSSLClient::connect::ioctlsocket(), to: " + baseUrl) << endl;
			}
			return false;
		}
#endif
		return true;
	}

	std::vector<TCPSSLClient*> TCPSSLClient::processIO(std::unordered_map<uint32_t, std::unique_ptr<WebSocketClient>>& shardMap) noexcept {
		std::vector<TCPSSLClient*> returnValue{};
		PollFDWrapper readWriteSet{};
		for (auto& [key, value]: shardMap) {
			if (value && value->areWeStillConnected()) {
				pollfd fdSet{ .fd = static_cast<SOCKET>(value->socket) };
				if (value->outputBuffer.getUsedSpace() > 0) {
					fdSet.events = POLLIN | POLLOUT;
				} else {
					fdSet.events = POLLIN;
				}
				readWriteSet.indices.emplace_back(key);
				readWriteSet.polls.emplace_back(fdSet);
			}
		}
		if (readWriteSet.polls.size() == 0) {
			return returnValue;
		}
		if (auto returnValueNew = poll(readWriteSet.polls.data(), static_cast<u_long>(readWriteSet.polls.size()), 1);
			returnValueNew == SOCKET_ERROR) {
			for (size_t x = 0; x < readWriteSet.polls.size(); ++x) {
				returnValue.emplace_back(shardMap[readWriteSet.indices[x]].get());
			}
			return returnValue;
		} else if (returnValueNew == 0) {
			return returnValue;
		}
		for (size_t x = 0; x < readWriteSet.polls.size(); ++x) {
			if (readWriteSet.polls[x].revents & POLLOUT || readWriteSet.polls[x].revents & POLLIN) {
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
			} else {
				returnValue.emplace_back(shardMap[readWriteSet.indices[x]].get());
				continue;
			}
		}
		return returnValue;
	}

	ProcessIOResult TCPSSLClient::writeData(std::string_view dataToWrite, bool priority) noexcept {
		if (this->areWeStillConnected()) {
			if (dataToWrite.size() > 0 && this->ssl) {
				if (priority && dataToWrite.size() < this->maxBufferSize) {
					this->outputBuffer.clear();
					this->outputBuffer.writeData(dataToWrite);
					if (!this->processWriteData()) {
						return ProcessIOResult::Error;
					}
				} else {
					uint64_t remainingBytes{ dataToWrite.size() };
					while (remainingBytes > 0) {
						uint64_t amountToCollect{};
						if (dataToWrite.size() >= this->maxBufferSize) {
							amountToCollect = this->maxBufferSize;
						} else {
							amountToCollect = dataToWrite.size();
						}
						this->outputBuffer.writeData({ dataToWrite.data(), amountToCollect });
						dataToWrite = std::basic_string_view{ dataToWrite.data() + amountToCollect, dataToWrite.size() - amountToCollect };
						remainingBytes = dataToWrite.size();
					}
				}
			}
			return ProcessIOResult::Success;
		} else {
			return ProcessIOResult::Error;
		}
	}

	ProcessIOResult TCPSSLClient::processIO(int32_t waitTimeInMs) noexcept {
		pollfd readWriteSet{ .fd = static_cast<SOCKET>(this->socket) };
		if (this->outputBuffer.getUsedSpace() > 0) {
			readWriteSet.events = POLLIN | POLLOUT;
		} else {
			readWriteSet.events = POLLIN;
		}
		if (auto returnValue = poll(&readWriteSet, 1, waitTimeInMs); returnValue == SOCKET_ERROR) {
			if (this->doWePrintErrorMessages) {
				cout << reportSSLError("TCPSSLClient::processIO()") << endl;
			}
			return ProcessIOResult::Error;
		} else if (returnValue == 0) {
			return ProcessIOResult::Success;
		} else {
			if (readWriteSet.revents & POLLERR || readWriteSet.revents & POLLHUP || readWriteSet.revents & POLLNVAL) {
				if (this->doWePrintErrorMessages) {
					cout << reportSSLError("TCPSSLClient::processIO()") << endl;
				}
				return ProcessIOResult::Error;
			}
			if (readWriteSet.revents & POLLIN) {
				if (!this->processReadData()) {
					if (this->doWePrintErrorMessages) {
						cout << reportSSLError("TCPSSLClient::processIO()") << endl;
					}
					return ProcessIOResult::Error;
				} else {
					if (!this->areWeAStandaloneSocket) {
						this->handleBuffer();
					}
				}
			}
			if (readWriteSet.revents & POLLOUT) {
				if (!this->processWriteData()) {
					if (this->doWePrintErrorMessages) {
						cout << reportSSLError("TCPSSLClient::processIO()") << endl;
					}
					return ProcessIOResult::Error;
				}
			}
		}
		return ProcessIOResult::Success;
	}

	std::basic_string_view<char> TCPSSLClient::getInputBuffer() noexcept {
		return this->inputBuffer.readData();
	}

	bool TCPSSLClient::areWeStillConnected() noexcept {
		if (static_cast<SOCKET*>(this->socket) && *this->socket != INVALID_SOCKET && this->ssl) {
			return true;
		} else {
			return false;
		}
	}

	bool TCPSSLClient::processWriteData() noexcept {
		if (this->outputBuffer.getUsedSpace() > 0) {
			uint64_t bytesToWrite{ this->outputBuffer.getCurrentTail()->getUsedSpace() };

			uint64_t writtenBytes{};
			auto returnValue{ SSL_write_ex(this->ssl, this->outputBuffer.readData().data(), bytesToWrite, &writtenBytes) };
			auto errorValue{ SSL_get_error(this->ssl, returnValue) };
			switch (errorValue) {
				case SSL_ERROR_WANT_READ: {
					[[fallthrough]];
				}
				case SSL_ERROR_WANT_WRITE: {
					[[fallthrough]];
				}
				case SSL_ERROR_NONE: {
					return true;
				}
				case SSL_ERROR_ZERO_RETURN: {
					if (this->doWePrintErrorMessages) {
						cout << reportSSLError("TCPSSLClient::processWriteData()", errorValue, this->ssl) << endl;
					}
					return false;
				}
				default: {
					if (this->doWePrintErrorMessages) {
						cout << reportSSLError("TCPSSLClient::processWriteData()", errorValue, this->ssl) << endl;
					}
					return false;
				}
			}
		}
		return true;
	}

	bool TCPSSLClient::processReadData() noexcept {
		if (!this->inputBuffer.isItFull()) {
			do {
				uint64_t readBytes{};
				uint64_t bytesToRead{ this->maxBufferSize };
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
							if (!this->areWeAStandaloneSocket) {
								this->handleBuffer();
							}
						}
						break;
					}
					case SSL_ERROR_ZERO_RETURN: {
						if (this->doWePrintErrorMessages) {
							cout << reportSSLError("TCPSSLClient::processReadData()", errorValue, this->ssl) << endl;
						}
						return false;
					}
					default: {
						if (this->doWePrintErrorMessages) {
							cout << reportSSLError("TCPSSLClient::processReadData()", errorValue, this->ssl) << endl;
						}
						return false;
					}
				}
			} while (this->areWeStillConnected() && SSL_pending(this->ssl) && !this->inputBuffer.isItFull());
		}
		if (!this->areWeAStandaloneSocket) {
			this->handleBuffer();
		}
		return true;
	}

	int64_t TCPSSLClient::getBytesRead() noexcept {
		return this->bytesRead;
	}

	UDPConnection::UDPConnection(DiscordCoreAPI::StreamType streamTypeNew, bool doWePrintErrorsNew) noexcept {
		this->doWePrintErrors = doWePrintErrorsNew;
		this->streamType = streamTypeNew;
	}

	bool UDPConnection::connect(const std::string& baseUrlNew, uint16_t portNew, std::stop_token token) noexcept {
		this->baseUrl = baseUrlNew;
		this->port = portNew;
		addrinfoWrapper hints{};
		hints->ai_family = AF_INET;
		hints->ai_socktype = SOCK_DGRAM;
		hints->ai_protocol = IPPROTO_UDP;

		if (this->socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); this->socket == INVALID_SOCKET) {
			if (this->doWePrintErrors) {
				cout << reportError("UDPConnection::connect::socket(), to: " + baseUrlNew) << endl;
			}
			return false;
		}

		std::unique_ptr<char> optVal{ std::make_unique<char>(1) };
		if (auto returnValue = setsockopt(this->socket, SOL_SOCKET, SO_REUSEADDR, optVal.get(), sizeof(optVal)); returnValue < 0) {
			if (this->doWePrintErrors) {
				cout << reportError("UDPConnection::connect::setsockopt(), to: " + baseUrlNew) << endl;
			}
			return false;
		}

#ifdef _WIN32
		u_long value02{ 1 };
		if (ioctlsocket(this->socket, FIONBIO, &value02)) {
			if (this->doWePrintErrors) {
				cout << reportError("UDPConnection::connect::ioctlsocket(), to: " + baseUrlNew) << endl;
			}
			return false;
		}
#else
		if (fcntl(this->socket, F_SETFL, fcntl(this->socket, F_GETFL, 0) | O_NONBLOCK)) {
			if (this->doWePrintErrors) {
				cout << reportError("UDPConnection::connect::ioctlsocket(), to: " + baseUrlNew) << endl;
			}
			return false;
		}
#endif

		if (this->streamType == DiscordCoreAPI::StreamType::None) {
			if (getaddrinfo(baseUrlNew.c_str(), std::to_string(portNew).c_str(), hints, this->address)) {
				if (this->doWePrintErrors) {
					cout << reportError("UDPConnection::connect::getaddrinfo(), to: " + baseUrlNew) << endl;
				}
				return false;
			}
			if (::connect(this->socket, this->address->ai_addr, static_cast<uint32_t>(this->address->ai_addrlen)) == SOCKET_ERROR) {
				if (this->doWePrintErrors) {
					cout << reportError("UDPConnection::connect::connect(), to: " + baseUrlNew) << endl;
				}
				return false;
			}
		} else if (this->streamType == DiscordCoreAPI::StreamType::Client) {
			if (getaddrinfo(baseUrlNew.c_str(), std::to_string(portNew).c_str(), hints, this->address)) {
				if (this->doWePrintErrors) {
					cout << reportError("UDPConnection::connect::getaddrinfo(), to: " + baseUrlNew) << endl;
				}
				return false;
			}
			std::string connectionString{ "connecting" };
			int32_t result{};
			while ((result == 0 || errno == EWOULDBLOCK || errno == EINPROGRESS) && !token.stop_requested()) {
				result = sendto(this->socket, connectionString.data(), static_cast<int32_t>(connectionString.size()), 0,
					this->address->ai_addr, static_cast<int32_t>(this->address->ai_addrlen));
				std::this_thread::sleep_for(1ns);
			}
			result = 0;
			while ((result == 0 || errno == EWOULDBLOCK || errno == EINPROGRESS) && !token.stop_requested()) {
				result = recvfrom(this->socket, connectionString.data(), static_cast<int32_t>(connectionString.size()), 0,
					this->address->ai_addr, reinterpret_cast<socklen_t*>(&this->address->ai_addrlen));
				std::this_thread::sleep_for(1ns);
			}
		} else {
			hints->ai_flags = AI_PASSIVE;
			if (getaddrinfo(nullptr, std::to_string(portNew).c_str(), hints, this->address)) {
				if (this->doWePrintErrors) {
					cout << reportError("UDPConnection::connect::getaddrinfo(), to: " + baseUrlNew) << endl;
				}
				return false;
			}
			if (auto result = bind(this->socket, this->address->ai_addr, static_cast<int32_t>(this->address->ai_addrlen)); result != 0) {
				if (this->doWePrintErrors) {
					cout << reportError("UDPConnection::connect::bind(), to: " + baseUrlNew) << endl;
				}
				return false;
			}
			std::string connectionString{};
			int32_t result{};
			connectionString.resize(10);
			while ((result == 0 || errno == EWOULDBLOCK || errno == EINPROGRESS) && !token.stop_requested()) {
				result = recvfrom(this->socket, connectionString.data(), static_cast<int32_t>(connectionString.size()), 0,
					this->address->ai_addr, reinterpret_cast<socklen_t*>(&this->address->ai_addrlen));
				std::this_thread::sleep_for(1ns);
			}
			connectionString = "connected1";
			result = 0;
			while ((result == 0 || errno == EWOULDBLOCK || errno == EINPROGRESS) && !token.stop_requested()) {
				result = sendto(this->socket, connectionString.data(), static_cast<int32_t>(connectionString.size()), 0,
					this->address->ai_addr, static_cast<int32_t>(this->address->ai_addrlen));
				std::this_thread::sleep_for(1ns);
			}
		}

		return true;
	}

	ProcessIOResult UDPConnection::processIO(ProcessIOType type) noexcept {
		if (this->socket == INVALID_SOCKET) {
			return ProcessIOResult::Success;
		}
		pollfd readWriteSet{};
		readWriteSet.fd = this->socket;
		if (type == ProcessIOType::Both && this->outputBuffer.getUsedSpace() > 0) {
			readWriteSet.events = POLLIN | POLLOUT;
		} else if (type == ProcessIOType::Read_Only || this->outputBuffer.getUsedSpace() == 0) {
			readWriteSet.events = POLLIN;
		} else if (type == ProcessIOType::Write_Only && this->outputBuffer.getUsedSpace() > 0) {
			readWriteSet.events = POLLOUT;
		}

		if (auto returnValue = poll(&readWriteSet, 1, 0); returnValue == SOCKET_ERROR) {
			if (this->doWePrintErrors) {
				cout << reportError("UDPConnection::processIO()") << endl;
			}
			return ProcessIOResult::Error;
		} else if (returnValue == 0) {
			return ProcessIOResult::Success;
		} else {
			if (readWriteSet.revents & POLLERR || readWriteSet.revents & POLLHUP || readWriteSet.revents & POLLNVAL) {
				if (this->doWePrintErrors) {
					cout << reportSSLError("UDPConnection::processIO()") << endl;
				}
				return ProcessIOResult::Error;
			}
			if (readWriteSet.revents & POLLIN) {
				if (!this->processReadData()) {
					if (this->doWePrintErrors) {
						cout << reportSSLError("UDPConnection::processIO()") << endl;
					}
					return ProcessIOResult::Error;
				}
			}
			if (readWriteSet.revents & POLLOUT) {
				if (!this->processWriteData()) {
					if (this->doWePrintErrors) {
						cout << reportSSLError("UDPConnection::processIO()") << endl;
					}
					return ProcessIOResult::Error;
				}
			}
		}
		return ProcessIOResult::Success;
	}

	void UDPConnection::writeData(std::basic_string_view<std::byte> dataToWrite) noexcept {
		if (this->areWeStillConnected()) {
			uint64_t remainingBytes{ dataToWrite.size() };
			while (remainingBytes > 0) {
				uint64_t amountToCollect{};
				if (dataToWrite.size() >= this->maxBufferSize) {
					amountToCollect = this->maxBufferSize;
				} else {
					amountToCollect = dataToWrite.size();
				}
				this->outputBuffer.writeData({ dataToWrite.data(), amountToCollect });
				dataToWrite = std::basic_string_view{ dataToWrite.data() + amountToCollect, dataToWrite.size() - amountToCollect };
				remainingBytes = dataToWrite.size();
			}
		}
	}

	std::basic_string_view<std::byte> UDPConnection::getInputBuffer() noexcept {
		return this->inputBuffer.readData();
	}

	bool UDPConnection::areWeStillConnected() noexcept {
		if (static_cast<SOCKET*>(this->socket) && this->socket != INVALID_SOCKET) {
			return true;
		} else {
			return false;
		}
	}

	bool UDPConnection::processWriteData() noexcept {
		if (this->outputBuffer.getUsedSpace() > 0) {
			auto bytesToWrite{ this->outputBuffer.getCurrentTail()->getUsedSpace() };
			auto writtenBytes{ sendto(this->socket, reinterpret_cast<const char*>(this->outputBuffer.readData().data()),
				static_cast<int32_t>(bytesToWrite), 0, this->address->ai_addr, static_cast<int32_t>(this->address->ai_addrlen)) };
			if (writtenBytes <= 0 && errno != EWOULDBLOCK && errno != EINPROGRESS) {
				return false;
			} else if (writtenBytes > 0) {
				return true;
			}
		}
		return true;
	}

	bool UDPConnection::processReadData() noexcept {
		int32_t readBytes{};
		do {
			if (!this->inputBuffer.isItFull()) {
				uint64_t bytesToRead{ this->maxBufferSize };
				readBytes = recvfrom(static_cast<SOCKET>(this->socket),
					reinterpret_cast<char*>(this->inputBuffer.getCurrentHead()->getCurrentHead()), static_cast<int32_t>(bytesToRead), 0,
					this->address->ai_addr, reinterpret_cast<socklen_t*>(&this->address->ai_addrlen));
				if (readBytes <= 0 && errno != EWOULDBLOCK && errno != EINPROGRESS) {
					return false;
				} else if (readBytes > 0) {
					this->inputBuffer.getCurrentHead()->modifyReadOrWritePosition(RingBufferAccessType::Write, readBytes);
					this->inputBuffer.modifyReadOrWritePosition(RingBufferAccessType::Write, 1);
					this->bytesRead += readBytes;
					this->handleAudioBuffer();
				}
			}
		} while (readBytes > 0);
		return true;
	}

	void UDPConnection::disconnect() noexcept {
		if (this->streamType != DiscordCoreAPI::StreamType ::None) {
			this->outputBuffer.clear();
			this->writeData(std::basic_string_view<std::byte>{ reinterpret_cast<const std::byte*>("goodbye") });
			this->processIO(ProcessIOType::Write_Only);
		}
		this->socket = INVALID_SOCKET;
		this->outputBuffer.clear();
		this->inputBuffer.clear();
	}

	UDPConnection::~UDPConnection() {
		this->disconnect();
	}

	SSL_CTXWrapper SSLConnectionInterface::context{};
	std::mutex SSLConnectionInterface::mutex{};
}