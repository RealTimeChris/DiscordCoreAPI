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
/// UDPConnection.hpp - Header file for the "udp connection" stuff.
/// Dec 12, 2021
/// https://discordcoreapi.com
/// \file UDPConnection.hpp
#pragma once

#include <discordcoreapi/Utilities/TCPConnection.hpp>
#include <discordcoreapi/Utilities/RingBuffer.hpp>
#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/Utilities/Base.hpp>
#include <discordcoreapi/CoRoutine.hpp>

namespace discord_core_api {

	namespace discord_core_internal {

		class DiscordCoreAPI_Dll udp_connection {
		  public:
			friend class voice_connection;

			DCA_INLINE udp_connection() {
				resampleVector.resize(maxBufferSize);
			}

			DCA_INLINE udp_connection& operator=(udp_connection&& other) noexcept {
				resampleVector = std::move(other.resampleVector);
				outputBuffer   = std::move(other.outputBuffer);
				inputBuffer	   = std::move(other.inputBuffer);
				address		   = std::move(other.address);
				baseUrl		   = std::move(other.baseUrl);
				socket		   = std::move(other.socket);
				currentStatus  = other.currentStatus;
				streamType	   = other.streamType;
				bytesRead	   = other.bytesRead;
				port		   = other.port;
				return *this;
			};

			DCA_INLINE udp_connection(udp_connection&& other) noexcept {
				*this = std::move(other);
			};

			DCA_INLINE udp_connection(const jsonifier::string& baseUrlNew, uint16_t portNew, stream_type streamTypeNew,
				std::coroutine_handle<discord_core_api::co_routine<void, false>::promise_type>* token) {
				resampleVector.resize(maxBufferSize);
				streamType = streamTypeNew;
				baseUrl	   = baseUrlNew;
				port	   = portNew;
				addrinfo_wrapper hints{};
				hints->ai_family   = AF_INET;
				hints->ai_socktype = SOCK_DGRAM;
				hints->ai_protocol = IPPROTO_UDP;

				if (socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); socket.operator SOCKET() == INVALID_SOCKET) {
					message_printer::printError<print_message_type::websocket>(reportError("connect::SOCKET(), to: " + baseUrlNew));
					currentStatus = connection_status::CONNECTION_Error;
					socket		  = INVALID_SOCKET;
					return;
				}

				unique_ptr<char> optVal{ makeUnique<char>(static_cast<char>(1)) };
				if (auto returnData = setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, optVal.get(), sizeof(optVal)); returnData < 0) {
					message_printer::printError<print_message_type::websocket>(reportError("connect::setsockopt(), to: " + baseUrlNew));
					currentStatus = connection_status::CONNECTION_Error;
					socket		  = INVALID_SOCKET;
					return;
				}

#if defined _WIN32
				u_long value02{ 1 };
				if (ioctlsocket(socket, FIONBIO, &value02)) {
					message_printer::printError<print_message_type::websocket>(reportError("connect::ioctlsocket(), to: " + baseUrlNew));
					currentStatus = connection_status::CONNECTION_Error;
					socket		  = INVALID_SOCKET;
					return;
				}
#else
				if (fcntl(socket, F_SETFL, fcntl(socket, F_GETFL, 0) | O_NONBLOCK)) {
					message_printer::printError<print_message_type::websocket>(reportError("connect::ioctlsocket(), to: " + baseUrlNew));
					currentStatus = connection_status::CONNECTION_Error;
					socket		  = INVALID_SOCKET;
					return;
				}
#endif

				if (streamType == stream_type::none) {
					if (getaddrinfo(baseUrlNew.data(), jsonifier::toString(portNew).data(), hints, address)) {
						message_printer::printError<print_message_type::websocket>(reportError("connect::getaddrinfo(), to: " + baseUrlNew));
						currentStatus = connection_status::CONNECTION_Error;
						socket		  = INVALID_SOCKET;
						return;
					}
					if (::connect(socket, address->ai_addr, static_cast<int32_t>(address->ai_addrlen)) == SOCKET_ERROR) {
						message_printer::printError<print_message_type::websocket>(reportError("connect::connect(), to: " + baseUrlNew));
						currentStatus = connection_status::CONNECTION_Error;
						socket		  = INVALID_SOCKET;
						return;
					}
				} else if (streamType == stream_type::client) {
					if (getaddrinfo(baseUrlNew.data(), jsonifier::toString(portNew).data(), hints, address)) {
						message_printer::printError<print_message_type::websocket>(reportError("connect::getaddrinfo(), to: " + baseUrlNew));
						currentStatus = connection_status::CONNECTION_Error;
						socket		  = INVALID_SOCKET;
						return;
					}
					static constexpr jsonifier::string_view connectionString{ "connecting" };
					jsonifier::string connectionStringNew{ "connecting" };
					int32_t result{};
					while ((result == 0 || errno == EWOULDBLOCK || errno == EINPROGRESS) && !token->promise().stopRequested()) {
						result =
							sendto(socket, connectionString.data(), static_cast<int32_t>(connectionString.size()), 0, address->ai_addr, static_cast<int32_t>(address->ai_addrlen));
						std::this_thread::sleep_for(1ns);
					}
					socklen_t currentSize{ static_cast<socklen_t>(address->ai_addrlen) };
					result = 0;
					while ((result == 0 || errno == EWOULDBLOCK || errno == EINPROGRESS) && !token->promise().stopRequested()) {
						result = recvfrom(socket, connectionStringNew.data(), static_cast<int32_t>(connectionString.size()), 0, address->ai_addr, &currentSize);
						std::this_thread::sleep_for(1ns);
					}
				} else {
					hints->ai_flags = AI_PASSIVE;
					if (getaddrinfo(nullptr, jsonifier::toString(portNew).data(), hints, address)) {
						message_printer::printError<print_message_type::websocket>(reportError("connect::getaddrinfo(), to: " + baseUrlNew));
						currentStatus = connection_status::CONNECTION_Error;
						socket		  = INVALID_SOCKET;
						return;
					}
					if (auto result = bind(socket, address->ai_addr, static_cast<int32_t>(address->ai_addrlen)); result != 0) {
						message_printer::printError<print_message_type::websocket>(reportError("connect::bind(), to: " + baseUrlNew));
						currentStatus = connection_status::CONNECTION_Error;
						socket		  = INVALID_SOCKET;
						return;
					}
					jsonifier::string connectionString{};
					int32_t result{};
					socklen_t currentSize{ static_cast<socklen_t>(address->ai_addrlen) };
					connectionString.resize(10);
					while ((result == 0 || errno == EWOULDBLOCK || errno == EINPROGRESS) && !token->promise().stopRequested()) {
						result = recvfrom(socket, connectionString.data(), static_cast<int32_t>(connectionString.size()), 0, address->ai_addr, &currentSize);
						std::this_thread::sleep_for(1ns);
					}
					connectionString = "connected1";
					result			 = 0;
					while ((result == 0 || errno == EWOULDBLOCK || errno == EINPROGRESS) && !token->promise().stopRequested()) {
						result =
							sendto(socket, connectionString.data(), static_cast<int32_t>(connectionString.size()), 0, address->ai_addr, static_cast<int32_t>(address->ai_addrlen));
						std::this_thread::sleep_for(1ns);
					}
				}
			}

			DCA_INLINE connection_status processIO() {
				if (!areWeStillConnected()) {
					return currentStatus;
				};
				pollfd readWriteSet{};
				readWriteSet.fd = static_cast<SOCKET>(socket);
				if (outputBuffer.getUsedSpace() > 0) {
					readWriteSet.events = POLLIN | POLLOUT;
				} else {
					readWriteSet.events = POLLIN;
				}
				if (auto returnValue = poll(&readWriteSet, 1, 0); returnValue == SOCKET_ERROR) {
					message_printer::printError<print_message_type::websocket>(reportSSLError("processIO() 00") + reportError("processIO() 00"));
					currentStatus = connection_status::SOCKET_Error;
					socket		  = INVALID_SOCKET;
					return currentStatus;
				} else if (returnValue == 0) {
					return currentStatus;
				} else {
					if (readWriteSet.revents & POLLOUT) {
						if (!processWriteData()) {
							message_printer::printError<print_message_type::websocket>(reportSSLError("processIO() 01") + reportError("processIO() 01"));
							currentStatus = connection_status::WRITE_Error;
							socket		  = INVALID_SOCKET;
							return currentStatus;
						}
					}
					if (readWriteSet.revents & POLLIN) {
						if (!processReadData()) {
							message_printer::printError<print_message_type::websocket>(reportSSLError("processIO() 02") + reportError("processIO() 02"));
							currentStatus = connection_status::READ_Error;
							socket		  = INVALID_SOCKET;
							return currentStatus;
						}
					}
					if (readWriteSet.revents & POLLERR) {
						message_printer::printError<print_message_type::websocket>(reportSSLError("processIO() 03") + reportError("processIO() 03"));
						currentStatus = connection_status::POLLERR_Error;
						socket		  = INVALID_SOCKET;
					}
					if (readWriteSet.revents & POLLNVAL) {
						message_printer::printError<print_message_type::websocket>(reportSSLError("processIO() 04") + reportError("processIO() 04"));
						currentStatus = connection_status::POLLNVAL_Error;
						socket		  = INVALID_SOCKET;
					}
					if (readWriteSet.revents & POLLHUP) {
						currentStatus = connection_status::POLLHUP_Error;
						socket		  = INVALID_SOCKET;
					}
				}
				return currentStatus;
			}

			DCA_INLINE void writeData(jsonifier::string_view_base<uint8_t> dataToWrite) {
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
						dataToWrite	   = jsonifier::string_view_base{ dataToWrite.data() + amountToCollect, dataToWrite.size() - amountToCollect };
						remainingBytes = dataToWrite.size();
					}
				}
			}

			DCA_INLINE jsonifier::string_view_base<uint8_t> getInputBuffer() {
				return inputBuffer.readData();
			}

			DCA_INLINE bool areWeStillConnected() {
				if (socket.operator SOCKET() != INVALID_SOCKET) {
					pollfd fdEvent = {};
					fdEvent.fd	   = socket;
					fdEvent.events = POLLOUT;
					int32_t result = poll(&fdEvent, 1, 1);
					if (result == SOCKET_ERROR || fdEvent.revents & POLLHUP || fdEvent.revents & POLLNVAL || fdEvent.revents & POLLERR) {
						socket = INVALID_SOCKET;
						return false;
					}
					return true;
				} else {
					return false;
				}
			}

			DCA_INLINE bool processWriteData() {
				if (outputBuffer.getUsedSpace() > 0) {
					auto bytesToWrite{ outputBuffer.getCurrentTail()->getUsedSpace() };
					auto newData = outputBuffer.readData();
					std::memcpy(resampleVector.data(), newData.data(), newData.size());
					auto writtenBytes{ sendto(socket, resampleVector.data(), static_cast<int32_t>(bytesToWrite), 0, address->ai_addr, static_cast<int32_t>(address->ai_addrlen)) };
					if (writtenBytes <= 0 && errno != EWOULDBLOCK && errno != EINPROGRESS) {
						return false;
					} else if (writtenBytes > 0) {
						return true;
					}
				}
				return true;
			}

			DCA_INLINE bool processReadData() {
				int32_t readBytes{};
				do {
					if (!inputBuffer.isItFull()) {
						socklen_t currentSize{ static_cast<socklen_t>(address->ai_addrlen) };
						uint64_t bytesToRead{ maxBufferSize };
						readBytes = recvfrom(static_cast<SOCKET>(socket), resampleVector.data(), static_cast<int32_t>(bytesToRead), 0, address->ai_addr, &currentSize);
						if (readBytes <= 0 && errno != EWOULDBLOCK && errno != EINPROGRESS) {
							return false;
						} else if (readBytes > 0) {
							inputBuffer.writeData(resampleVector.data(), static_cast<uint64_t>(readBytes));
							bytesRead += readBytes;
							handleAudioBuffer();
						}
					}
				} while (readBytes > 0);
				return true;
			}

			virtual DCA_INLINE void handleAudioBuffer() = 0;

			virtual DCA_INLINE void disconnect() {
				socket = INVALID_SOCKET;
				outputBuffer.clear();
				inputBuffer.clear();
			}

			virtual DCA_INLINE ~udp_connection() {
				disconnect();
			}

		  protected:
			static constexpr uint64_t maxBufferSize{ (1024 * 16) };
			jsonifier::vector<char> resampleVector{};
			ring_buffer<uint8_t, 16> outputBuffer{};
			ring_buffer<uint8_t, 16> inputBuffer{};
			connection_status currentStatus{};
			jsonifier::string baseUrl{};
			addrinfo_wrapper address{};
			stream_type streamType{};
			socket_wrapper socket{};
			int64_t bytesRead{};
			uint16_t port{};
		};
	}
}
