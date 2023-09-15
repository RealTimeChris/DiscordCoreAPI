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

		/**
		 * \addtogroup discord_core_internal
		 * @{
		 */

		/// @brief Structure to hold the encoded data and sample count returned by the encoder.
		struct EncoderReturnData {
			std::basic_string_view<uint8_t> data{};///< Encoded data.
			uint64_t sampleCount{};///< Number of audio samples in the input frame.
		};

		/// @brief Wrapper class for the Opus audio encoder.
		struct OpusEncoderWrapper {
			/// @brief Deleter for OpusEncoder instances.
			struct OpusEncoderDeleter {
				/// @brief Operator to destroy an OpusEncoder instance.
				/// @param other The OpusEncoder pointer to be deleted.
				inline void operator()(OpusEncoder* other) {
					if (other) {
						opus_encoder_destroy(other);
						other = nullptr;
					}
				}
			};

			/// @brief Constructor for OpusEncoderWrapper. Initializes and configures the Opus encoder.
			inline OpusEncoderWrapper() {
				int32_t error{};
				ptr.reset(opus_encoder_create(sampleRate, nChannels, OPUS_APPLICATION_AUDIO, &error));

				// Set Opus signal type to music
				auto result = opus_encoder_ctl(ptr.get(), OPUS_SET_SIGNAL(OPUS_SIGNAL_MUSIC));
				if (result != OPUS_OK) {
					throw DCAException{ "Failed to set the Opus signal type, Reason: " + std::string{ opus_strerror(result) } };
				}

				// Set Opus bitrate to maximum
				result = opus_encoder_ctl(ptr.get(), OPUS_SET_BITRATE(OPUS_BITRATE_MAX));
				if (result != OPUS_OK) {
					throw DCAException{ "Failed to set the Opus bitrate, Reason: " + std::string{ opus_strerror(result) } };
				}
			}

			/// @brief Encode Opus audio data.
			/// @param inputFrame The audio data to encode.
			/// @return Encoded data and sample count.
			/// @throws DCAException if encoding fails.
			inline EncoderReturnData encodeData(std::basic_string_view<uint8_t> inputFrame) {
				if (inputFrame.size() == 0) {
					return {};
				}

				if (encodedData.size() == 0) {
					encodedData.resize(maxBufferSize);
				}
				if (resampleVector.size() < inputFrame.size() / 2) {
					resampleVector.resize(inputFrame.size() / 2);
				}
				std::memcpy(resampleVector.data(), inputFrame.data(), inputFrame.size());
				uint64_t sampleCount = inputFrame.size() / 2 / 2;
				int32_t count		 = opus_encode(ptr.get(), resampleVector.data(), static_cast<int32_t>(inputFrame.size() / 2 / 2), encodedData.data(), maxBufferSize);
				if (count <= 0) {
					throw DCAException{ "Failed to encode the bitstream, Reason: " + std::string{ opus_strerror(count) } };
				}

				EncoderReturnData returnData{};
				returnData.sampleCount = sampleCount;
				returnData.data		   = std::basic_string_view<uint8_t>{ encodedData.data(), encodedData.size() };
				return returnData;
			}

		  protected:
			UniquePtr<OpusEncoder, OpusEncoderDeleter> ptr{};///< Unique pointer to OpusEncoder instance.
			jsonifier::vector<opus_int16> resampleVector{};///< For properly copying the values without type-punning.
			std::basic_string<uint8_t> encodedData{};///< Buffer for encoded audio data.
			inline static constexpr uint64_t maxBufferSize{ 1276 };///< Maximum size of the encoded data buffer.
			inline static constexpr int64_t sampleRate{ 48000 };///< Sample rate of the audio data.
			inline static constexpr int64_t nChannels{ 2 };///< Number of audio channels.
		};

		/**@}*/

	}

}
