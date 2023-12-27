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

namespace discord_core_api {

	namespace discord_core_internal {

		/**
		 * \addtogroup discord_core_internal
		 * @{
		 */

		/// @brief Structure to hold the encoded data and sample count returned by the encoder.
		struct encoder_return_data {
			jsonifier::string_view_base<uint8_t> data{};///< Encoded data.
			uint64_t sampleCount{};///< Number of audio samples in the input frame.
		};

		/// @brief Wrapper class for the opus audio encoder.
		struct opus_encoder_wrapper {
			/// @brief Deleter for OpusEncoder instances.
			struct opus_encoder_deleter {
				/// @brief Operator to destroy an OpusEncoder instance.
				/// @param other the OpusEncoder pointer to be deleted.
				DCA_INLINE void operator()(OpusEncoder* other) {
					if (other) {
						opus_encoder_destroy(other);
						other = nullptr;
					}
				}
			};

			/// @brief Constructor for opus_encoder_wrapper. initializes and configures the opus encoder.
			DCA_INLINE opus_encoder_wrapper() {
				int32_t error{};
				ptr.reset(opus_encoder_create(sampleRate, nChannels, OPUS_APPLICATION_AUDIO, &error));

				// set opus signal type to music
				auto result = opus_encoder_ctl(ptr.get(), OPUS_SET_SIGNAL(OPUS_SIGNAL_MUSIC));
				if (result != OPUS_OK) {
					throw dca_exception{ "Failed to set the opus signal type, reason: " + jsonifier::string{ opus_strerror(result) } };
				}

				// set opus bitrate to maximum
				result = opus_encoder_ctl(ptr.get(), OPUS_SET_BITRATE(OPUS_BITRATE_MAX));
				if (result != OPUS_OK) {
					throw dca_exception{ "Failed to set the opus bitrate, reason: " + jsonifier::string{ opus_strerror(result) } };
				}
			}

			/// @brief Encode opus audio data.
			/// @param inputFrame the audio data to encode.
			/// @return encoded data and sample count.
			/// @throws dca_exception if encoding fails.
			DCA_INLINE encoder_return_data encodeData(jsonifier::string_view_base<uint8_t> inputFrame) {
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
					throw dca_exception{ "Failed to encode the bitstream, reason: " + jsonifier::string{ opus_strerror(count) } };
				}

				encoder_return_data returnData{};
				returnData.sampleCount = sampleCount;
				returnData.data		   = jsonifier::string_view_base<uint8_t>{ encodedData.data(), encodedData.size() };
				return returnData;
			}

		  protected:
			unique_ptr<OpusEncoder, opus_encoder_deleter> ptr{};///< Unique pointer to OpusEncoder instance.
			jsonifier::vector<opus_int16> resampleVector{};///< For properly copying the values without type-punning.
			jsonifier::string_base<uint8_t> encodedData{};///< Buffer for encoded audio data.
			static constexpr uint64_t maxBufferSize{ 1276 };///< Maximum size of the encoded data buffer.
			static constexpr int64_t sampleRate{ 48000 };///< Sample rate of the audio data.
			static constexpr int64_t nChannels{ 2 };///< Number of audio channels.
		};

		/**@}*/

	}

}
