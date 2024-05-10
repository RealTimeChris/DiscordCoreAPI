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
/// AVX512.hpp - Header for the AVX512 instruction set.
/// Nov 8, 2021
/// https://discordcoreapi.com
/// \file AVX512.hpp
///
#pragma once

#include <discordcoreapi/Utilities/Base.hpp>

#if DCA_CHECK_FOR_INSTRUCTION(DCA_AVX512) && !DCA_CHECK_FOR_INSTRUCTION(DCA_AVX) && !DCA_CHECK_FOR_INSTRUCTION(DCA_AVX2)

	#include <immintrin.h>
	#include <numeric>

namespace discord_core_api {

	namespace discord_core_internal {

		using avx512_float = __m512;

		// @brief A class for audio mixing operations using AVX512 instructions.
		class audio_mixer {
		  public:
			// @brief The number of 32-bit values per cpu register.
			static constexpr int32_t byteBlocksPerRegister{ 16 };

			// @brief Collect a single register worth of data from dataIn, apply gain and increment, and store the result in dataOut. this version uses AVX512 instructions.
			// @param dataIn pointer to the input array of int32_t values.
			// @param dataOut pointer to the output array of int16_t values.
			// @param currentGain the gain to be applied to the elements.
			// @param increment the increment value to be added to each element.
			DCA_INLINE void collectSingleRegister(const int32_t* dataIn, int16_t* dataOut, const float currentGain, const float increment) {
				currentSamples = _mm512_mul_ps(gatherValues(dataIn),
					_mm512_add_ps(_mm512_set1_ps(currentGain),
						_mm512_mul_ps(_mm512_set1_ps(increment),
							_mm512_set_ps(0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f))));

				currentSamples = _mm512_mask_blend_ps(_mm512_cmp_ps_mask(currentSamples, _mm512_set1_ps(0.0f), _CMP_GE_OQ),
					_mm512_max_ps(currentSamples, _mm512_set1_ps(static_cast<float>(std::numeric_limits<int16_t>::min()))),
					_mm512_min_ps(currentSamples, _mm512_set1_ps(static_cast<float>(std::numeric_limits<int16_t>::max()))));

				storeValues(currentSamples, dataOut);
			}

			// @brief Combine a register worth of elements from decodedData and store the result in upSampledVector. this version uses AVX512 instructions.
			// @param upSampledVector pointer to the array of int32_t values.
			// @param decodedData pointer to the array of int16_t values.
			DCA_INLINE void combineSamples(const int16_t* decodedData, int32_t* upSampledVector) {
				storeValues(_mm512_add_ps(gatherValues(upSampledVector), gatherValues(decodedData)), upSampledVector);
			}

		  protected:
			alignas(64) float newArray[byteBlocksPerRegister]{};
			avx512_float currentSamples{};

			// @brief Stores values from a 512-bit AVX512 vector to a storage location.
			// @tparam value_type the target value type for storage.
			// @param valuesToStore the 512-bit AVX512 vector containing values to store.
			// @param storageLocation pointer to the storage location.
			template<typename value_type> DCA_INLINE void storeValues(const avx512_float& valuesToStore, value_type* storageLocation) {
				_mm512_store_ps(newArray, valuesToStore);
				for (int64_t x = 0; x < byteBlocksPerRegister; ++x) {
					storageLocation[x] = static_cast<value_type>(newArray[x]);
				}
			}

			// @brief Specialization for gathering non-float values into an AVX512 register.
			// @tparam value_type the type of values being gathered.
			// @tparam indices parameter pack of indices for gathering values.
			// @return an AVX512 register containing gathered values.
			template<typename value_type> DCA_INLINE avx512_float gatherValues(const value_type* values) {
				for (uint64_t x = 0; x < byteBlocksPerRegister; ++x) {
					newArray[x] = static_cast<float>(values[x]);
				}
				return _mm512_load_ps(newArray);
			}
		};

	};
}

#endif
