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
/// Fallback.hpp - Header for the fallback instruction set.
/// Nov 8, 2021
/// https://discordcoreapi.com
/// \file Fallback.hpp
#pragma once

#include <discordcoreapi/Utilities/Base.hpp>

#if (!DCA_CHECK_FOR_INSTRUCTION(DCA_AVX)) && (!DCA_CHECK_FOR_INSTRUCTION(DCA_AVX2)) && (!DCA_CHECK_FOR_INSTRUCTION(DCA_AVX512))

	#include <cstdint>
	#include <numeric>

namespace discord_core_api {

	namespace discord_core_internal {

		// @brief A class for audio mixing operations using AVX instructions.
		class audio_mixer {
		  public:
			// @brief The number of 32-bit values per cpu register.
			static constexpr int32_t byteBlocksPerRegister{ 2 };

			// @brief Collect a single register worth of data from dataIn, apply gain and increment, and store the result in dataOut. this version uses x64 instructions.
			// @param dataIn pointer to the input array of int32_t values.
			// @param dataOut pointer to the output array of int16_t values.
			// @param currentGain the gain to be applied to the elements.
			// @param increment the increment value to be added to each element.
			DCA_INLINE void collectSingleRegister(const int32_t* dataIn, int16_t* dataOut, const float currentGain, const float increment) {
				for (uint64_t x = 0; x < byteBlocksPerRegister; ++x) {
					incrementNew	   = increment * static_cast<float>(x);
					currentGainNew	 = currentGain + incrementNew;
					currentSampleNew   = static_cast<float>(dataIn[x]) * currentGainNew;
					if (currentSampleNew >= static_cast<float>(std::numeric_limits<int16_t>::max())) {
						currentSampleNew = static_cast<float>(std::numeric_limits<int16_t>::max());
					} else if (currentSampleNew <= static_cast<float>(std::numeric_limits<int16_t>::min())) {
						currentSampleNew = static_cast<float>(std::numeric_limits<int16_t>::min());
					}
					dataOut[x] = static_cast<int16_t>(currentSampleNew);
				}
			}

			// @brief Combine a register worth of elements from decodedData and store the result in upSampledVector. this version uses x64 instructions.
			// @param upSampledVector pointer to the array of int32_t values.
			// @param decodedData pointer to the array of int16_t values.
			DCA_INLINE static void combineSamples(const int16_t* decodedData, int32_t* upSampledVector) {
				for (uint64_t x = 0; x < byteBlocksPerRegister; ++x) {
					upSampledVector[x] += static_cast<int32_t>(decodedData[x]);
				}
			}

		  protected:
			float currentSampleNew{};
			float currentGainNew{};
			float incrementNew{};
		};
	}
}

#endif
