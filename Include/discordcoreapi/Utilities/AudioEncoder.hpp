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
/// AudioEncoder.hpp - Header for the audio encoder class.
/// Aug 22, 2021
/// https://discordcoreapi.com
/// \file AudioEncoder.hpp

#pragma once

#include <discordcoreapi/FoundationEntities.hpp>
#include <opus/opus.h>

namespace DiscordCoreAPI {

	namespace DiscordCoreInternal {

		struct DiscordCoreAPI_Dll EncoderReturnData {
			std::basic_string_view<uint8_t> data{};
			uint64_t sampleCount{};
		};

		struct DiscordCoreAPI_Dll OpusEncoderWrapper {
			struct DiscordCoreAPI_Dll OpusEncoderDeleter {
				inline void operator()(OpusEncoder* other) {
					if (other) {
						opus_encoder_destroy(other);
						other = nullptr;
					}
				}
			};

			inline OpusEncoderWrapper() {
				int32_t error{};
				ptr.reset(opus_encoder_create(sampleRate, nChannels, OPUS_APPLICATION_AUDIO, &error));
				auto result = opus_encoder_ctl(ptr.get(), OPUS_SET_SIGNAL(OPUS_SIGNAL_MUSIC));
				if (result != OPUS_OK) {
					throw DCAException{ "Failed to set the Opus signal type, Reason: " + std::string{ opus_strerror(result) },
						std::source_location::current() };
				}
				result = opus_encoder_ctl(ptr.get(), OPUS_SET_BITRATE(OPUS_BITRATE_MAX));
				if (result != OPUS_OK) {
					throw DCAException{ "Failed to set the Opus bitrate, Reason: " + std::string{ opus_strerror(result) },
						std::source_location::current() };
				}
			}

			inline EncoderReturnData encodeData(std::basic_string_view<uint8_t> inputFrame) {
				if (inputFrame.size() == 0) {
					return {};
				}
				if (encodedData.size() == 0) {
					encodedData.resize(maxBufferSize);
				}
				uint64_t sampleCount = inputFrame.size() / 2 / 2;
				int32_t count = opus_encode(ptr.get(), reinterpret_cast<const opus_int16*>(inputFrame.data()),
					static_cast<int32_t>(inputFrame.size() / 2 / 2), reinterpret_cast<uint8_t*>(encodedData.data()), maxBufferSize);
				if (count <= 0) {
					throw DCAException{ "Failed to encode the bitstream, Reason: " + std::string{ opus_strerror(count) },
						std::source_location::current() };
				}
				EncoderReturnData returnData{};
				returnData.sampleCount = sampleCount;
				returnData.data = std::basic_string_view<uint8_t>{ encodedData.data(), encodedData.size() };
				return returnData;
			}

		  protected:
			UniquePtr<OpusEncoder, OpusEncoderDeleter> ptr{};
			std::basic_string<uint8_t> encodedData{};
			const int32_t maxBufferSize{ 1276 };
			const int32_t sampleRate{ 48000 };
			const int32_t nChannels{ 2 };
		};

	}
}
