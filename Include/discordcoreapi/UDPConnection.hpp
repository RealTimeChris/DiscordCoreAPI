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
/// UDPConnection.hpp - Header file for the "UDP Connection" stuff.
/// Dec 12, 2021
/// https://discordcoreapi.com
/// \file UDPConnection.hpp

#pragma once

#include <discordcoreapi/Utilities/RingBuffer.hpp>
#include <discordcoreapi/FoundationEntities.hpp>
#include <discordcoreapi/TCPConnection.hpp>

namespace DiscordCoreAPI {

	namespace DiscordCoreInternal {

		class UDPConnection {
		  public:
			friend class VoiceConnection;

			UDPConnection() noexcept = default;

			UDPConnection& operator=(UDPConnection&& other) noexcept;
			UDPConnection(UDPConnection&& other) noexcept;

			UDPConnection(const std::string& baseUrlNew, uint16_t portNew, StreamType streamType, std::stop_token token = std::stop_token{});

			void writeData(std::basic_string_view<uint8_t> dataToWrite);

			std::basic_string_view<uint8_t> getInputBuffer() noexcept;

			virtual void handleAudioBuffer() noexcept = 0;

			ConnectionStatus processIO() noexcept;

			bool areWeStillConnected() noexcept;

			void disconnect() noexcept;

			bool processWriteData();

			bool processReadData();

			~UDPConnection() noexcept;

		  protected:
			const uint64_t maxBufferSize{ (1024 * 16) };
			StreamType streamType{};
			RingBuffer<uint8_t, 16> outputBuffer{};
			RingBuffer<uint8_t, 16> inputBuffer{};
			ConnectionStatus currentStatus{};
			addrinfoWrapper address{};
			SOCKETWrapper socket{};
			std::string baseUrl{};
			int64_t bytesRead{};
			uint16_t port{};
		};
	}
}
