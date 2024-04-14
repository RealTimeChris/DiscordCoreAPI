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

#if JSONIFIER_CHECK_FOR_INSTRUCTION(JSONIFIER_AVX) && !JSONIFIER_CHECK_FOR_INSTRUCTION(JSONIFIER_AVX2) && !JSONIFIER_CHECK_FOR_INSTRUCTION(JSONIFIER_AVX512)

	#include <immintrin.h>
	#include <numeric>

namespace discord_core_api {

	namespace discord_core_internal {

		using avx_float = __m128;

		// @brief A class for audio mixing operations using AVX2 instructions.
		class audio_mixer {
		  public:
			// @brief The number of 32-bit values per cpu register.
			static constexpr int32_t byteBlocksPerRegister{ 4 };

			// @brief Collect a single register worth of data from dataIn, apply gain and increment, and store the result in dataOut. this version uses AVX instructions.
			// @param dataIn pointer to the input array of int32_t values.
			// @param dataOut pointer to the output array of int16_t values.
			// @param currentGain the gain to be applied to the elements.
			// @param increment the increment value to be added to each element.
			DCA_INLINE void collectSingleRegister(const int32_t* dataIn, int16_t* dataOut, const float currentGain, const float increment) {
				currentSamples = _mm_mul_ps(gatherValues(dataIn), _mm_add_ps(_mm_set1_ps(currentGain), _mm_mul_ps(_mm_set1_ps(increment), _mm_set_ps(0.0f, 1.0f, 2.0f, 3.0f))));

				currentSamples = _mm_blendv_ps(_mm_max_ps(currentSamples, _mm_set1_ps(static_cast<float>(std::numeric_limits<int16_t>::min()))),
					_mm_min_ps(currentSamples, _mm_set1_ps(static_cast<float>(std::numeric_limits<int16_t>::max()))),
					_mm_cmp_ps(currentSamples, _mm_set1_ps(0.0f), _CMP_GE_OQ));

				storeValues(currentSamples, dataOut);
			}

			// @brief Combine a register worth of elements from decodedData and store the result in upSampledVector. this version uses AVX instructions.
			// @param upSampledVector pointer to the array of int32_t values.
			// @param decodedData pointer to the array of int16_t values.
			DCA_INLINE void combineSamples(const int16_t* decodedData, int32_t* upSampledVector) {
				storeValues(_mm_add_ps(gatherValues(upSampledVector), gatherValues(decodedData)), upSampledVector);
			}

		  protected:
			alignas(16) float newArray[byteBlocksPerRegister]{};
			avx_float currentSamples{};

			// @brief Stores values from a 128-bit AVX vector to a storage location.
			// @tparam value_type the target value type for storage.
			// @param valuesToStore the 128-bit AVX vector containing values to store.
			// @param storageLocation pointer to the storage location.
			template<typename value_type> DCA_INLINE void storeValues(const avx_float& valuesToStore, value_type* storageLocation) {
				_mm_store_ps(newArray, valuesToStore);
				for (int64_t x = 0; x < byteBlocksPerRegister; ++x) {
					storageLocation[x] = static_cast<value_type>(newArray[x]);
				}
			}

			// @brief Specialization for gathering non-float values into an AVX register.
			// @tparam value_type the type of values being gathered.
			// @tparam indices parameter pack of indices for gathering values.
			// @return an AVX register containing gathered values.
			template<typename value_type> DCA_INLINE avx_float gatherValues(const value_type* values) {
				for (uint64_t x = 0; x < byteBlocksPerRegister; ++x) {
					newArray[x] = static_cast<float>(values[x]);
				}
				return _mm_load_ps(newArray);
			}
		};

	}
}

#endif
