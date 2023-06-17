/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022, 2023 Chris M. (RealTimeChris)

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
/// AudioEncoder.hpp - Header for the audio encoder class.
/// Aug 22, 2021
/// https://discordcoreapi.com
/// \file AudioEncoder.hpp

#pragma once

#include <discordcoreapi/FoundationEntities.hpp>
#include <opus/opus.h>

namespace DiscordCoreInternal {

	struct DiscordCoreAPI_Dll EncoderReturnData {
		std::basic_string_view<uint8_t> data{};
		uint64_t sampleCount{};
	};

	struct DiscordCoreAPI_Dll OpusEncoderWrapper {
		struct DiscordCoreAPI_Dll OpusEncoderDeleter {
			inline void operator()(OpusEncoder* other) noexcept {
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
				throw DiscordCoreAPI::DCAException{ "Failed to set the Opus signal type, Reason: " + std::string{ opus_strerror(result) } };
			}
			result = opus_encoder_ctl(ptr.get(), OPUS_SET_BITRATE(OPUS_BITRATE_MAX));
			if (result != OPUS_OK) {
				throw DiscordCoreAPI::DCAException{ "Failed to set the Opus bitrate, Reason: " + std::string{ opus_strerror(result) } };
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
				throw DiscordCoreAPI::DCAException{ "Failed to encode the bitstream, Reason: " + std::string{ opus_strerror(count) } };
			}
			EncoderReturnData returnData{};
			returnData.sampleCount = sampleCount;
			returnData.data = std::basic_string_view<uint8_t>{ encodedData.data(), encodedData.size() };
			return returnData;
		}

	  protected:
		std::unique_ptr<OpusEncoder, OpusEncoderDeleter> ptr{ nullptr, OpusEncoderDeleter{} };
		std::basic_string<uint8_t> encodedData{};
		const int32_t maxBufferSize{ 1276 };
		const int32_t sampleRate{ 48000 };
		const int32_t nChannels{ 2 };
	};

}
