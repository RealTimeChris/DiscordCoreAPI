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
/// ISADetection.hpp - Header for the detection of the AVX instruction set.
/// Nov 8, 2021
/// https://discordcoreapi.com
/// \file ISADetection.hpp

#pragma once

#include <immintrin.h>
#include <mmintrin.h>
#include <limits>

namespace DiscordCoreAPI {

	namespace DiscordCoreInternal {
		/**
		 * \addtogroup discord_core_internal
		 * @{
		 */

		using Avx512Float = __m512;
		using Avx512Int = __m512i;
		using Avx2Float = __m256;
		using Avx2Int = __m256i;
		using AvxFloat = __m128;
		using AvxInt = __m128i;

		/// @brief Extracts a 32-bit integer from a 128-bit AVX2 register.
		/// @param value The AVX2 register containing packed 16-bit integers.
		/// @param index The index of the 32-bit integer to extract (0-3).
		/// @return The extracted 32-bit integer.
		inline int32_t extractInt32FromAvx(const AvxInt& value, int64_t index) {
			switch (index) {
				case 0: {
					return _mm_extract_epi32(value, 0);
				}
				case 1: {
					return _mm_extract_epi32(value, 1);
				}
				case 2: {
					return _mm_extract_epi32(value, 2);
				}
				case 3: {
					return _mm_extract_epi32(value, 3);
				}
				default: {
					return _mm_extract_epi32(value, 0);
				}
			}
		}

		/// @brief Extracts a 32-bit integer from a 256-bit AVX2 register.
		/// @param value The AVX2 register containing packed 32-bit integers.
		/// @param index The index of the 32bit integer to extract (0-7).
		/// @return The extracted 32-bit integer.
		inline int32_t extractInt32FromAvx2(const Avx2Int& value, int64_t index) {
			switch (index) {
				case 0: {
					return _mm256_extract_epi32(value, 0);
				}
				case 1: {
					return _mm256_extract_epi32(value, 1);
				}
				case 2: {
					return _mm256_extract_epi32(value, 2);
				}
				case 3: {
					return _mm256_extract_epi32(value, 3);
				}
				case 4: {
					return _mm256_extract_epi32(value, 4);
				}
				case 5: {
					return _mm256_extract_epi32(value, 5);
				}
				case 6: {
					return _mm256_extract_epi32(value, 6);
				}
				case 7: {
					return _mm256_extract_epi32(value, 7);
				}
				default: {
					return _mm256_extract_epi32(value, 0);
				}
			}
		}

		/// @brief Extracts a 32-bit integer from a 512-bit AVX-512 register.
		/// @param value The AVX-512 register containing packed 16-bit integers.
		/// @param index The index of the 32-bit integer to extract (0-15).
		/// @return The extracted 32-bit integer.
		inline int32_t extractInt32FromAvx512(const Avx512Int& value, int64_t index) {
			alignas(64) int32_t result[16];
			_mm512_store_si512(result, value);
			return result[index];
		}

#ifdef T_AVX512

		/// @brief A class for audio mixing operations using AVX512 instructions.
		class AudioMixer {
		  public:
			/// @brief The number of 32-bit values per CPU register.
			static constexpr int32_t byteBlocksPerRegister{ 16 };

			/// @brief Stores values from a 512-bit AVX vector to a storage location.
			/// @tparam ValueType The target value type for storage.
			/// @param valuesToStore The 512-bit AVX vector containing values to store.
			/// @param storageLocation Pointer to the storage location.
			template<typename ValueType> inline static void storeValues(const Avx512Int& valuesToStore, ValueType* storageLocation) {
				for (int64_t x = 0; x < byteBlocksPerRegister; ++x) {
					storageLocation[x] = static_cast<ValueType>(extractInt32FromAvx512(valuesToStore, x));
				}
			}

			/// @brief Specialization for gathering non-float values into an AVX register.
			/// @tparam ValueType The type of values being gathered.
			/// @param values The values to gather.
			/// @return An AVX register containing gathered values.
			template<typename ValueType> inline static Avx512Float gatherValues(ValueType* values) {
				float newArray[byteBlocksPerRegister]{};
				for (uint64_t x = 0; x < byteBlocksPerRegister; ++x) {
					newArray[x] = static_cast<float>(values[x]);
				}
				return _mm512_loadu_ps(newArray);
			}

			/// @brief Collect a single register worth of data from dataIn, apply gain and increment, and store the result in dataOut. This version uses AVX512 instructions.
			/// @param dataIn Pointer to the input array of int32_t values.
			/// @param dataOut Pointer to the output array of int16_t values.
			/// @param currentGain The gain to be applied to the elements.
			/// @param increment The increment value to be added to each element.
			inline static void collectSingleRegister(int32_t* dataIn, int16_t* dataOut, float currentGain, float increment) {
				Avx512Float currentSamplesNew{ _mm512_mul_ps(gatherValues(dataIn),
					_mm512_add_ps(_mm512_set1_ps(currentGain),
						_mm512_mul_ps(_mm512_set1_ps(increment),
							_mm512_set_ps(0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f)))) };

				currentSamplesNew = _mm512_mask_blend_ps(_mm512_cmp_ps_mask(currentSamplesNew, _mm512_set1_ps(0.0f), _CMP_GE_OQ),
					_mm512_max_ps(currentSamplesNew, _mm512_set1_ps(static_cast<float>(std::numeric_limits<opus_int16>::min()))),
					_mm512_min_ps(currentSamplesNew, _mm512_set1_ps(static_cast<float>(std::numeric_limits<opus_int16>::max()))));

				storeValues(_mm512_cvtps_epi32(currentSamplesNew), dataOut);
			}

			/// @brief Combine a register worth of elements from decodedData and store the result in upSampledVector. This version uses AVX51 instructions.
			/// @param upSampledVector Pointer to the array of int32_t values.
			/// @param decodedData Pointer to the array of int16_t values.
			inline static void combineSamples(int32_t* upSampledVector, const int16_t* decodedData) {
				auto newValues{ _mm512_cvtps_epi32(_mm512_add_ps(gatherValues(upSampledVector), gatherValues(decodedData))) };
				storeValues(newValues, upSampledVector);
			}
		};

#elif T_AVX2

		/// @brief A class for audio mixing operations using AVX2 instructions.
		class AudioMixer {
		  public:
			/// @brief The number of 32-bit values per CPU register.
			static constexpr int32_t byteBlocksPerRegister{ 8 };

			/// @brief Stores values from a 256-bit AVX vector to a storage location.
			/// @tparam ValueType The target value type for storage.
			/// @param valuesToStore The 256-bit AVX vector containing values to store.
			/// @param storageLocation Pointer to the storage location.
			template<typename ValueType> inline static void storeValues(const Avx2Int& valuesToStore, ValueType* storageLocation) {
				for (int64_t x = 0; x < byteBlocksPerRegister; ++x) {
					storageLocation[x] = static_cast<ValueType>(extractInt32FromAvx2(valuesToStore, x));
				}
			}

			/// @brief Specialization for gathering non-float values into an AVX register.
			/// @tparam ValueType The type of values being gathered.
			/// @param values The values to gather.
			/// @return An AVX register containing gathered values.
			template<typename ValueType> inline static Avx2Float gatherValues(ValueType* values) {
				float newArray[byteBlocksPerRegister]{};
				for (uint64_t x = 0; x < byteBlocksPerRegister; ++x) {
					newArray[x] = static_cast<float>(values[x]);
				}
				return _mm256_loadu_ps(newArray);
			}

			/// @brief Collect a single register worth of data from dataIn, apply gain and increment, and store the result in dataOut. This version uses AVX2 instructions.
			/// @param dataIn Pointer to the input array of int32_t values.
			/// @param dataOut Pointer to the output array of int16_t values.
			/// @param currentGain The gain to be applied to the elements.
			/// @param increment The increment value to be added to each element.
			inline static void collectSingleRegister(int32_t* dataIn, int16_t* dataOut, float currentGain, float increment) {
				Avx2Float currentSamplesNew{ _mm256_mul_ps(gatherValues(dataIn),
					_mm256_add_ps(_mm256_set1_ps(currentGain),
						_mm256_mul_ps(_mm256_set1_ps(increment), _mm256_set_ps(0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f)))) };

				currentSamplesNew =
					_mm256_blendv_ps(_mm256_max_ps(currentSamplesNew, _mm256_set1_ps(static_cast<float>(std::numeric_limits<opus_int16>::min()))),
						_mm256_min_ps(currentSamplesNew, _mm256_set1_ps(static_cast<float>(std::numeric_limits<opus_int16>::max()))),
						_mm256_cmp_ps(currentSamplesNew, _mm256_set1_ps(0.0f), _CMP_GE_OQ));

				storeValues(_mm256_cvtps_epi32(currentSamplesNew), dataOut);
			}

			/// @brief Combine a register worth of elements from decodedData and store the result in upSampledVector. This version uses AVX2 instructions.
			/// @param upSampledVector Pointer to the array of int32_t values.
			/// @param decodedData Pointer to the array of int16_t values.
			inline static void combineSamples(int32_t* upSampledVector, const int16_t* decodedData) {
				auto newValues{ _mm256_cvtps_epi32(_mm256_add_ps(gatherValues(upSampledVector), gatherValues(decodedData))) };
				storeValues(newValues, upSampledVector);
			}
		};

#elif T_AVX

		/// @brief A class for audio mixing operations using AVX instructions.
		class AudioMixer {
		  public:
			/// @brief The number of 32-bit values per CPU register.
			static constexpr int32_t byteBlocksPerRegister{ 4 };

			/// @brief Stores values from a 128-bit AVX vector to a storage location.
			/// @tparam ValueType The target value type for storage.
			/// @param valuesToStore The 128-bit AVX vector containing values to store.
			/// @param storageLocation Pointer to the storage location.
			template<typename ValueType> inline static void storeValues(const AvxInt& valuesToStore, ValueType* storageLocation) {
				for (int64_t x = 0; x < byteBlocksPerRegister; ++x) {
					storageLocation[x] = static_cast<ValueType>(extractInt32FromAvx(valuesToStore, x));
				}
			}

			/// @brief Specialization for gathering non-float values into an AVX register.
			/// @tparam ValueType The type of values being gathered.
			/// @param values The values to gather.
			/// @return An AVX register containing gathered values.
			template<typename ValueType> inline static AvxFloat gatherValues(ValueType* values) {
				float newArray[byteBlocksPerRegister]{};
				for (uint64_t x = 0; x < byteBlocksPerRegister; ++x) {
					newArray[x] = static_cast<float>(values[x]);
				}
				return _mm_loadu_ps(newArray);
			}

			/// @brief Collect a single register worth of data from dataIn, apply gain and increment, and store the result in dataOut. This version uses AVX instructions.
			/// @param dataIn Pointer to the input array of int32_t values.
			/// @param dataOut Pointer to the output array of int16_t values.
			/// @param currentGain The gain to be applied to the elements.
			/// @param increment The increment value to be added to each element.
			inline static void collectSingleRegister(int32_t* dataIn, int16_t* dataOut, float currentGain, float increment) {
				AvxFloat currentSamplesNew{ _mm_mul_ps(gatherValues(dataIn),
					_mm_add_ps(_mm_set1_ps(currentGain), _mm_mul_ps(_mm_set1_ps(increment), _mm_set_ps(0.0f, 1.0f, 2.0f, 3.0f)))) };

				currentSamplesNew =
					_mm_blendv_ps(_mm_max_ps(currentSamplesNew, _mm_set1_ps(static_cast<float>(std::numeric_limits<opus_int16>::min()))),
						_mm_min_ps(currentSamplesNew, _mm_set1_ps(static_cast<float>(std::numeric_limits<opus_int16>::max()))),
						_mm_cmp_ps(currentSamplesNew, _mm_set1_ps(0.0f), _CMP_GE_OQ));

				storeValues(_mm_cvtps_epi32(currentSamplesNew), dataOut);
			}

			/// @brief Combine a register worth of elements from decodedData and store the result in upSampledVector. This version uses AVX instructions.
			/// @param upSampledVector Pointer to the array of int32_t values.
			/// @param decodedData Pointer to the array of int16_t values.
			inline static void combineSamples(int32_t* upSampledVector, const int16_t* decodedData) {
				auto newValues{ _mm_cvtps_epi32(_mm_add_ps(gatherValues(upSampledVector), gatherValues(decodedData))) };
				storeValues(newValues, upSampledVector);
			}
		};

#else

		/// @brief A class for audio mixing operations using AVX instructions.
		class AudioMixer {
		  public:
			/// @brief The number of 32-bit values per CPU register.
			static constexpr int32_t byteBlocksPerRegister{ 2 };

			/// @brief Collect a single register worth of data from dataIn, apply gain and increment, and store the result in dataOut. This version uses x64 instructions.
			/// @param dataIn Pointer to the input array of int32_t values.
			/// @param dataOut Pointer to the output array of int16_t values.
			/// @param currentGain The gain to be applied to the elements.
			/// @param increment The increment value to be added to each element.
			inline static void collectSingleRegister(int32_t* dataIn, int16_t* dataOut, float currentGain, float increment) {
				for (uint64_t x = 0; x < byteBlocksPerRegister; ++x) {
					auto incrementNew = increment * x;
					auto currentGainNew = currentGain + incrementNew;
					auto currentSampleNew = dataIn[x] * currentGainNew;
					if (currentSampleNew >= std::numeric_limits<int16_t>::max()) {
						currentSampleNew = std::numeric_limits<int16_t>::max();
					} else if (currentSampleNew <= std::numeric_limits<int16_t>::min()) {
						currentSampleNew = std::numeric_limits<int16_t>::min();
					}
					dataOut[x] = static_cast<int16_t>(currentSampleNew);
				}
			}

			/// @brief Combine a register worth of elements from decodedData and store the result in upSampledVector. This version uses x64 instructions.
			/// @param upSampledVector Pointer to the array of int32_t values.
			/// @param decodedData Pointer to the array of int16_t values.
			inline static void combineSamples(int32_t* upSampledVector, const int16_t* decodedData) {
				for (uint64_t x = 0; x < byteBlocksPerRegister; ++x) {
					upSampledVector[x] += static_cast<int32_t>(decodedData[x]);
				}
			}
		};

#endif
		/**@}*/

	}
}
