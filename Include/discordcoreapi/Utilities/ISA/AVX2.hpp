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
/// AVX2.hpp - Header for the AVX2 instruction set.
/// Nov 8, 2021
/// https://discordcoreapi.com
/// \file AVX2.hpp
#pragma once

#if JSONIFIER_CHECK_FOR_INSTRUCTION(JSONIFIER_AVX2) && !JSONIFIER_CHECK_FOR_INSTRUCTION(JSONIFIER_AVX) && !JSONIFIER_CHECK_FOR_INSTRUCTION(JSONIFIER_AVX512)

	#include <immintrin.h>
	#include <numeric>

namespace DiscordCoreAPI {

	namespace DiscordCoreInternal {

		using Avx2Float = __m256;

		// @brief A class for audio mixing operations using AVX2 instructions.
		class AudioMixer {
		  public:
			// @brief The number of 32-bit values per CPU register.
			inline static constexpr int32_t byteBlocksPerRegister{ 8 };

			// @brief Collect a single register worth of data from dataIn, apply gain and increment, and store the result in dataOut. This version uses AVX2 instructions.
			// @param dataIn Pointer to the input array of int32_t values.
			// @param dataOut Pointer to the output array of int16_t values.
			// @param currentGain The gain to be applied to the elements.
			// @param increment The increment value to be added to each element.
			inline void collectSingleRegister(const int32_t* dataIn, int16_t* dataOut, const float currentGain, const float increment) {
				Avx2Float currentSamplesNew{ _mm256_mul_ps(gatherValues(dataIn),
					_mm256_add_ps(_mm256_set1_ps(currentGain), _mm256_mul_ps(_mm256_set1_ps(increment), _mm256_set_ps(0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f)))) };

				currentSamplesNew = _mm256_blendv_ps(_mm256_max_ps(currentSamplesNew, _mm256_set1_ps(static_cast<float>(std::numeric_limits<int16_t>::min()))),
					_mm256_min_ps(currentSamplesNew, _mm256_set1_ps(static_cast<float>(std::numeric_limits<int16_t>::max()))),
					_mm256_cmp_ps(currentSamplesNew, _mm256_set1_ps(0.0f), _CMP_GE_OQ));

				storeValues(currentSamplesNew, dataOut);
			}

			// @brief Combine a register worth of elements from decodedData and store the result in upSampledVector. This version uses AVX2 instructions.
			// @param upSampledVector Pointer to the array of int32_t values.
			// @param decodedData Pointer to the array of int16_t values.
			inline void combineSamples(const int16_t* decodedData, int32_t* upSampledVector) {
				storeValues(_mm256_add_ps(gatherValues(upSampledVector), gatherValues(decodedData)), upSampledVector);
			}

		  protected:
			alignas(32) float newArray[byteBlocksPerRegister]{};

			// @brief Stores values from a 256-bit AVX2 vector to a storage location.
			// @tparam ValueType The target value type for storage.
			// @param valuesToStore The 256-bit AVX2 vector containing values to store.
			// @param storageLocation Pointer to the storage location.
			template<typename ValueType> inline void storeValues(const Avx2Float& valuesToStore, ValueType* storageLocation) {
				_mm256_store_ps(newArray, valuesToStore);
				for (int64_t x = 0; x < byteBlocksPerRegister; ++x) {
					storageLocation[x] = static_cast<ValueType>(newArray[x]);
				}
			}

			// @brief Specialization for gathering non-float values into an AVX2 register.
			// @tparam ValueType The type of values being gathered.
			// @tparam Indices Parameter pack of indices for gathering values.
			// @return An AVX2 register containing gathered values.
			template<typename ValueType> inline Avx2Float gatherValues(const ValueType* values) {
				for (uint64_t x = 0; x < byteBlocksPerRegister; ++x) {
					newArray[x] = static_cast<float>(values[x]);
				}
				return _mm256_load_ps(newArray);
			}
		};
	}
}

#endif