/*
	MIT License

	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2022, 2023 Chris M. (RealTimeChris)

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/
/// UDPConnection.cpp - Source file for the "UDP Connection" stuff.
/// Dec 12, 2021
/// https://discordcoreapi.com
/// \file UDPConnection.cpp

#include <discordcoreapi/UDPConnection.hpp>
#include <discordcoreapi/TCPConnection.hpp>
#include <discordcoreapi/WebSocketEntities.hpp>

namespace DiscordCoreAPI {

	namespace DiscordCoreInternal {

		UDPConnection& UDPConnection::operator=(UDPConnection&& other) noexcept {
			outputBuffer = std::move(other.outputBuffer);
			inputBuffer = std::move(other.inputBuffer);
			currentStatus = other.currentStatus;
			address = std::move(other.address);
			baseUrl = std::move(other.baseUrl);
			socket = std::move(other.socket);
			streamType = other.streamType;
			bytesRead = other.bytesRead;
			port = other.port;
			return *this;
		};

		UDPConnection::UDPConnection(UDPConnection&& other) noexcept {
			*this = std::move(other);
		};

		UDPConnection::UDPConnection(const std::string& baseUrlNew, uint16_t portNew, StreamType streamTypeNew, std::stop_token token) {
			streamType = streamTypeNew;
			baseUrl = baseUrlNew;
			port = portNew;
			addrinfoWrapper hints{};
			hints->ai_family = AF_INET;
			hints->ai_socktype = SOCK_DGRAM;
			hints->ai_protocol = IPPROTO_UDP;

			if (socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); socket.operator SOCKET() == INVALID_SOCKET) {
				MessagePrinter::printError<PrintMessageType::WebSocket>(reportError("UDPConnection::connect::socket(), to: " + baseUrlNew));
				currentStatus = ConnectionStatus::CONNECTION_Error;
				socket = INVALID_SOCKET;
				return;
			}

			UniquePtr<char> optVal{ makeUnique<char>(static_cast<char>(1)) };
			if (auto returnData = setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, optVal.get(), sizeof(optVal)); returnData < 0) {
				MessagePrinter::printError<PrintMessageType::WebSocket>(reportError("UDPConnection::connect::setsockopt(), to: " + baseUrlNew));
				currentStatus = ConnectionStatus::CONNECTION_Error;
				socket = INVALID_SOCKET;
				return;
			}

#ifdef _WIN32
			u_long value02{ 1 };
			if (ioctlsocket(socket, FIONBIO, &value02)) {
				MessagePrinter::printError<PrintMessageType::WebSocket>(reportError("UDPConnection::connect::ioctlsocket(), to: " + baseUrlNew));
				currentStatus = ConnectionStatus::CONNECTION_Error;
				socket = INVALID_SOCKET;
				return;
			}
#else
			if (fcntl(socket, F_SETFL, fcntl(socket, F_GETFL, 0) | O_NONBLOCK)) {
				MessagePrinter::printError<PrintMessageType::WebSocket>(reportError("UDPConnection::connect::ioctlsocket(), to: " + baseUrlNew));
				currentStatus = ConnectionStatus::CONNECTION_Error;
				socket = INVALID_SOCKET;
				return;
			}
#endif

			if (streamType == StreamType::None) {
				if (getaddrinfo(baseUrlNew.c_str(), std::to_string(portNew).c_str(), hints, address)) {
					MessagePrinter::printError<PrintMessageType::WebSocket>(reportError("UDPConnection::connect::getaddrinfo(), to: " + baseUrlNew));
					currentStatus = ConnectionStatus::CONNECTION_Error;
					socket = INVALID_SOCKET;
					return;
				}
				if (::connect(socket, address->ai_addr, static_cast<uint32_t>(address->ai_addrlen)) == SOCKET_ERROR) {
					MessagePrinter::printError<PrintMessageType::WebSocket>(reportError("UDPConnection::connect::connect(), to: " + baseUrlNew));
					currentStatus = ConnectionStatus::CONNECTION_Error;
					socket = INVALID_SOCKET;
					return;
				}
			} else if (streamType == StreamType::Client) {
				if (getaddrinfo(baseUrlNew.c_str(), std::to_string(portNew).c_str(), hints, address)) {
					MessagePrinter::printError<PrintMessageType::WebSocket>(reportError("UDPConnection::connect::getaddrinfo(), to: " + baseUrlNew));
					currentStatus = ConnectionStatus::CONNECTION_Error;
					socket = INVALID_SOCKET;
					return;
				}
				std::string connectionString{ "connecting" };
				int32_t result{};
				while ((result == 0 || errno == EWOULDBLOCK || errno == EINPROGRESS) && !token.stop_requested()) {
					result = sendto(socket, connectionString.data(), static_cast<int32_t>(connectionString.size()), 0, address->ai_addr,
						static_cast<int32_t>(address->ai_addrlen));
					std::this_thread::sleep_for(1ns);
				}
				result = 0;
				while ((result == 0 || errno == EWOULDBLOCK || errno == EINPROGRESS) && !token.stop_requested()) {
					result = recvfrom(socket, connectionString.data(), static_cast<int32_t>(connectionString.size()), 0, address->ai_addr,
						reinterpret_cast<socklen_t*>(&address->ai_addrlen));
					std::this_thread::sleep_for(1ns);
				}
			} else {
				hints->ai_flags = AI_PASSIVE;
				if (getaddrinfo(nullptr, std::to_string(portNew).c_str(), hints, address)) {
					MessagePrinter::printError<PrintMessageType::WebSocket>(reportError("UDPConnection::connect::getaddrinfo(), to: " + baseUrlNew));
					currentStatus = ConnectionStatus::CONNECTION_Error;
					socket = INVALID_SOCKET;
					return;
				}
				if (auto result = bind(socket, address->ai_addr, static_cast<int32_t>(address->ai_addrlen)); result != 0) {
					MessagePrinter::printError<PrintMessageType::WebSocket>(reportError("UDPConnection::connect::bind(), to: " + baseUrlNew));
					currentStatus = ConnectionStatus::CONNECTION_Error;
					socket = INVALID_SOCKET;
					return;
				}
				std::string connectionString{};
				int32_t result{};
				connectionString.resize(10);
				while ((result == 0 || errno == EWOULDBLOCK || errno == EINPROGRESS) && !token.stop_requested()) {
					result = recvfrom(socket, connectionString.data(), static_cast<int32_t>(connectionString.size()), 0, address->ai_addr,
						reinterpret_cast<socklen_t*>(&address->ai_addrlen));
					std::this_thread::sleep_for(1ns);
				}
				connectionString = "connected1";
				result = 0;
				while ((result == 0 || errno == EWOULDBLOCK || errno == EINPROGRESS) && !token.stop_requested()) {
					result = sendto(socket, connectionString.data(), static_cast<int32_t>(connectionString.size()), 0, address->ai_addr,
						static_cast<int32_t>(address->ai_addrlen));
					std::this_thread::sleep_for(1ns);
				}
			}
		}

		ConnectionStatus UDPConnection::processIO() noexcept {
			if (!areWeStillConnected()) {
				return currentStatus;
			};
			pollfd readWriteSet{ .fd = static_cast<SOCKET>(socket) };
			if (outputBuffer.getUsedSpace() > 0) {
				readWriteSet.events = POLLIN | POLLOUT;
			} else {
				readWriteSet.events = POLLIN;
			}
			if (auto returnValue = poll(&readWriteSet, 1, 0); returnValue == SOCKET_ERROR) {
				MessagePrinter::printError<PrintMessageType::WebSocket>(
					reportSSLError("UDPConnection::processIO() 00") + reportError("UDPConnection::processIO() 00"));
				currentStatus = ConnectionStatus::SOCKET_Error;
				socket = INVALID_SOCKET;
				return currentStatus;
			} else if (returnValue == 0) {
				return currentStatus;
			} else {
				if (readWriteSet.revents & POLLOUT) {
					if (!processWriteData()) {
						MessagePrinter::printError<PrintMessageType::WebSocket>(
							reportSSLError("UDPConnection::processIO() 01") + reportError("UDPConnection::processIO() 01"));
						currentStatus = ConnectionStatus::WRITE_Error;
						socket = INVALID_SOCKET;
						return currentStatus;
					}
				}
				if (readWriteSet.revents & POLLIN) {
					if (!processReadData()) {
						MessagePrinter::printError<PrintMessageType::WebSocket>(
							reportSSLError("UDPConnection::processIO() 02") + reportError("UDPConnection::processIO() 02"));
						currentStatus = ConnectionStatus::READ_Error;
						socket = INVALID_SOCKET;
						return currentStatus;
					}
				}
				if (readWriteSet.revents & POLLERR) {
					MessagePrinter::printError<PrintMessageType::WebSocket>(
						reportSSLError("UDPConnection::processIO() 03") + reportError("UDPConnection::processIO() 03"));
					currentStatus = ConnectionStatus::POLLERR_Error;
					socket = INVALID_SOCKET;
				}
				if (readWriteSet.revents & POLLNVAL) {
					MessagePrinter::printError<PrintMessageType::WebSocket>(
						reportSSLError("UDPConnection::processIO() 04") + reportError("UDPConnection::processIO() 04"));
					currentStatus = ConnectionStatus::POLLNVAL_Error;
					socket = INVALID_SOCKET;
				}
				if (readWriteSet.revents & POLLHUP) {
					currentStatus = ConnectionStatus::POLLHUP_Error;
					socket = INVALID_SOCKET;
				}
			}
			return currentStatus;
		}

		void UDPConnection::writeData(std::basic_string_view<uint8_t> dataToWrite) {
			if (areWeStillConnected()) {
				uint64_t remainingBytes{ dataToWrite.size() };
				while (remainingBytes > 0) {
					uint64_t amountToCollect{};
					if (dataToWrite.size() >= maxBufferSize) {
						amountToCollect = maxBufferSize;
					} else {
						amountToCollect = dataToWrite.size();
					}
					outputBuffer.writeData(dataToWrite.data(), amountToCollect);
					dataToWrite = std::basic_string_view{ dataToWrite.data() + amountToCollect, dataToWrite.size() - amountToCollect };
					remainingBytes = dataToWrite.size();
				}
			}
		}

		std::basic_string_view<uint8_t> UDPConnection::getInputBuffer() noexcept {
			return inputBuffer.readData();
		}

		bool UDPConnection::areWeStillConnected() noexcept {
			if (socket.operator SOCKET() != INVALID_SOCKET) {
				return true;
			} else {
				return false;
			}
		}

		bool UDPConnection::processWriteData() {
			if (outputBuffer.getUsedSpace() > 0) {
				auto bytesToWrite{ outputBuffer.getCurrentTail()->getUsedSpace() };
				auto writtenBytes{ sendto(socket, reinterpret_cast<const char*>(outputBuffer.readData().data()), static_cast<int32_t>(bytesToWrite),
					0, address->ai_addr, static_cast<int32_t>(address->ai_addrlen)) };
				if (writtenBytes <= 0 && errno != EWOULDBLOCK && errno != EINPROGRESS) {
					return false;
				} else if (writtenBytes > 0) {
					return true;
				}
			}
			return true;
		}

		bool UDPConnection::processReadData() {
			int32_t readBytes{};
			do {
				if (!inputBuffer.isItFull()) {
					uint64_t bytesToRead{ maxBufferSize };
					readBytes = recvfrom(static_cast<SOCKET>(socket), reinterpret_cast<char*>(inputBuffer.getCurrentHead()->getCurrentHead()),
						static_cast<int32_t>(bytesToRead), 0, address->ai_addr, reinterpret_cast<socklen_t*>(&address->ai_addrlen));
					if (readBytes <= 0 && errno != EWOULDBLOCK && errno != EINPROGRESS) {
						return false;
					} else if (readBytes > 0) {
						inputBuffer.getCurrentHead()->modifyReadOrWritePosition(RingBufferAccessType::Write, readBytes);
						inputBuffer.modifyReadOrWritePosition(RingBufferAccessType::Write, 1);
						bytesRead += readBytes;
						handleAudioBuffer();
					}
				}
			} while (readBytes > 0);
			return true;
		}

		void UDPConnection::disconnect() noexcept {
			if (streamType != StreamType::None) {
				outputBuffer.clear();
				writeData(std::basic_string_view<uint8_t>{ reinterpret_cast<const uint8_t*>("goodbye") });
				try {
					processIO();
				} catch (const DCAException& error) {
					MessagePrinter::printError<PrintMessageType::WebSocket>(error.what());
				}
			}
			socket = INVALID_SOCKET;
			outputBuffer.clear();
			inputBuffer.clear();
		}

		UDPConnection::~UDPConnection() noexcept {
			disconnect();
		}

	}
}
