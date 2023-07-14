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
/// AudioDecoder.hpp - Header for the audio decoder class.
/// Jul 29, 2021
/// https://discordcoreapi.com
/// \file AudioDecoder.hpp

#pragma once

#include <discordcoreapi/FoundationEntities.hpp>
#include <opus/opus.h>

namespace DiscordCoreAPI {

	namespace DiscordCoreInternal {

		struct DiscordCoreAPI_Dll OpusDecoderWrapper {
			struct DiscordCoreAPI_Dll OpusDecoderDeleter {
				inline void operator()(OpusDecoder* other) {
					if (other) {
						opus_decoder_destroy(other);
						other = nullptr;
					}
				}
			};

			inline OpusDecoderWrapper() {
				int32_t error{};
				data.resize(23040);
				ptr.reset(opus_decoder_create(48000, 2, &error));
				if (error != OPUS_OK) {
					throw DCAException{ "Failed to create the Opus decoder, Reason: " + std::string{ opus_strerror(error) },
						std::source_location::current() };
				}
			}

			inline std::basic_string_view<opus_int16> decodeData(std::basic_string_view<uint8_t> dataToDecode) {
				const int64_t sampleCount =
					opus_decode(ptr.get(), dataToDecode.data(), static_cast<opus_int32>(dataToDecode.length() & 0x7FFFFFFF), data.data(), 5760, 0);
				if (sampleCount > 0) {
					return std::basic_string_view<opus_int16>{ data.data(), static_cast<uint64_t>(sampleCount * 2ull) };
				} else {
					throw DCAException{ "Failed to decode a user's voice payload, Reason: " + std::string{ opus_strerror(sampleCount) },
						std::source_location::current() };
				}
			}

		  protected:
			UniquePtr<OpusDecoder, OpusDecoderDeleter> ptr{};
			std::vector<opus_int16> data{};
		};
	}
}
