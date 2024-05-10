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
/// AVX.hpp - Header for the AVX instruction set.
/// Nov 8, 2021
/// https://discordcoreapi.com
/// \file AVX.hpp
#pragma once

#include <discordcoreapi/Utilities/Base.hpp>

#if DCA_CHECK_FOR_INSTRUCTION(DCA_NEON)

#include <arm_neon.h>

namespace discord_core_api {

	namespace discord_core_internal {

		using neon_float = float32x4_t ;

		// @brief A class for audio mixing operations using ARM NEON instructions.
		class audio_mixer {
		  public:
			// @brief The number of 32-bit values per cpu register.
			static constexpr int32_t byteBlocksPerRegister{ 4 };

			// @brief Collect a single register worth of data from dataIn, apply gain and increment, and store the result in dataOut. This version uses NEON instructions.
			// @param dataIn pointer to the input array of int32_t values.
			// @param dataOut pointer to the output array of int16_t values.
			// @param currentGain the gain to be applied to the elements.
			// @param increment the increment value to be added to each element.
			inline void collectSingleRegister(const int32_t* dataIn, int16_t* dataOut, const float currentGain, const float increment) {
				neon_float currentSamples =
					vmulq_f32(gatherValues(dataIn), vmlaq_f32(vdupq_n_f32(currentGain), vdupq_n_f32(increment), vcombine_f32(vcreate_f32(0x00000001), vcreate_f32(0x00000002))));

				currentSamples =
					vbslq_f32(vcgeq_f32(currentSamples, vdupq_n_f32(0.0f)), vminq_f32(currentSamples, vdupq_n_f32(static_cast<float>(std::numeric_limits<int16_t>::max()))),
						vmaxq_f32(currentSamples, vdupq_n_f32(static_cast<float>(std::numeric_limits<int16_t>::min()))));

				storeValues(currentSamples, dataOut);
			}

			// @brief Combine a register worth of elements from decodedData and store the result in upSampledVector. This version uses NEON instructions.
			// @param upSampledVector pointer to the array of int32_t values.
			// @param decodedData pointer to the array of int16_t values.
			inline void combineSamples(const int16_t* decodedData, int32_t* upSampledVector) {
				storeValues(vaddq_f32(gatherValues(upSampledVector), gatherValues(decodedData)), upSampledVector);
			}

		  protected:
			alignas(16) float newArray[byteBlocksPerRegister]{};
			neon_float currentSamples{};

			// @brief Stores values from a 128-bit NEON vector to a storage location.
			// @tparam value_type the target value type for storage.
			// @param valuesToStore the 128-bit NEON vector containing values to store.
			// @param storageLocation pointer to the storage location.
			template<typename value_type> inline void storeValues(const float32x4_t& valuesToStore, value_type* storageLocation) {
				vst1q_f32(newArray, valuesToStore);
				for (int64_t x = 0; x < byteBlocksPerRegister; ++x) {
					storageLocation[x] = static_cast<value_type>(newArray[x]);
				}
			}

			// @brief Specialization for gathering non-float values into a NEON register.
			// @tparam value_type the type of values being gathered.
			// @return a NEON register containing gathered values.
			template<typename value_type> inline neon_float gatherValues(const value_type* values) {
				for (uint64_t x = 0; x < byteBlocksPerRegister; ++x) {
					newArray[x] = static_cast<float>(values[x]);
				}
				return vld1q_f32(newArray);
			}
		};


	}
}

#endif