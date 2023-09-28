# 	MIT License
# 
# 	Copyright (c) 2023 RealTimeChris
# 
# 	Permission is hereby granted, free of charge, to any person obtaining a copy of this
# 	software and associated documentation files (the "Software"), to deal in the Software
# 	without restriction, including without limitation the rights to use, copy, modify, merge,
# 	publish, distribute, sublicense, and/or sell copies of the Software, and to permit
# 	persons to whom the Software is furnished to do so, subject to the following conditions:
# 
# 	The above copyright notice and this permission notice shall be included in all copies or
# 	substantial portions of the Software.
# 
# 	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
# 	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
# 	PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
# 	FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
# 	OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
# 	DEALINGS IN THE SOFTWARE.
# JsonifierDetectArchitecture.cmake - Script for detecting the CPU architecture.
# Sept 18, 2023
# https://discordcoreapi.com
include(CheckCXXSourceRuns)

function(jsonifier_check_instruction_set INSTRUCTION_SET_NAME INSTRUCTION_SET_IN_FLAG INSTRUCTION_SET_OUT_FLAG INSTRUCTION_SET_INTRINSIC)
    set(INSTRUCTION_SET_CODE "
    #include <immintrin.h>
    #include <stdint.h>
        
    int32_t main()
    {
        ${INSTRUCTION_SET_INTRINSIC};
        return 0;
    }")

    set(CMAKE_REQUIRED_FLAGS "${INSTRUCTION_SET_IN_FLAG}")
    set(CHECK_RESULT_VAR "${INSTRUCTION_SET_NAME}")
    CHECK_CXX_SOURCE_RUNS("${INSTRUCTION_SET_CODE}" "DiscordCoreAPI-${CHECK_RESULT_VAR}")
    if(DiscordCoreAPI-${CHECK_RESULT_VAR})
        set(${INSTRUCTION_SET_NAME} "${INSTRUCTION_SET_OUT_FLAG}" PARENT_SCOPE)
        list(APPEND JSONIFIER_CPU_INSTRUCTIONS "${INSTRUCTION_SET_NAME}")
        string(REPLACE ";" "," JSONIFIER_CPU_INSTRUCTIONS "${JSONIFIER_CPU_INSTRUCTIONS}")
        set(JSONIFIER_CPU_INSTRUCTIONS "${JSONIFIER_CPU_INSTRUCTIONS}" PARENT_SCOPE)
    else()
        message(STATUS "Instruction set ${INSTRUCTION_SET_NAME} not supported.")
        return()
    endif()
endfunction()

set(INSTRUCTION_SET_NAMES "POPCNT" "LZCNT" "BMI" "AVX" "AVX2" "AVX512")
set (INSTRUCTION_SET_CODE
    "auto result = _mm_popcnt_u64(uint64_t{})"
    "auto result = _lzcnt_u64(int64_t{})"
    "auto result = _blsr_u64(uint64_t{})"
    "auto result = _mm_castsi128_pd(__m128i{}).auto result02 = _mm_setzero_si128()"
    "auto result = _mm256_add_epi32(__m256i{}, __m256i{})"
    "auto result = _mm512_add_ps(__m512i{}, __m512i{}).auto result2 = _mm512_cmplt_epu8_mask(__m512i{}, __m512i{}).auto result03 = _mm_abs_epi64 (__m128i{})"
)

set(INDEX_SET "0" "1" "2" "3" "4" "5")

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(INSTRUCTION_SET_IN_FLAGS "/arch:AVX" "/arch:AVX" "/arch:AVX" "/arch:AVX" "/arch:AVX2" "/arch:AVX512")
    set(INSTRUCTION_SET_OUT_FLAGS "/arch:AVX" "/arch:AVX" "/arch:AVX" "/arch:AVX" "/arch:AVX2" "/arch:AVX512")
else()
    set(INSTRUCTION_SET_IN_FLAGS "-march=native" "-march=native" "-march=native" "-march=native" "-march=native" "-march=native")
    set(INSTRUCTION_SET_OUT_FLAGS "-mpopcnt" "-mlzcnt" "-mbmi" "-mavx.-mpclmul" "-mavx2.-mpclmul" "-mavx512f.-mavx512bw.-mavx512vl.-mpclmul")
endif()

set(CMAKE_REQUIRED_FLAGS_SAVE "${CMAKE_REQUIRED_FLAGS}")

if ((${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64") OR (${CMAKE_SYSTEM_PROCESSOR} MATCHES "i386") OR (${CMAKE_SYSTEM_PROCESSOR} MATCHES "AMD64"))

	foreach(CURRENT_INDEX IN LISTS INDEX_SET)
		list(GET INSTRUCTION_SET_NAMES "${CURRENT_INDEX}" INSTRUCTION_SET_NAME)
        list(GET INSTRUCTION_SET_CODE "${CURRENT_INDEX}" INSTRUCTION_SET_INTRINSIC)
        list(GET INSTRUCTION_SET_IN_FLAGS "${CURRENT_INDEX}" INSTRUCTION_SET_IN_FLAG)
        list(GET INSTRUCTION_SET_OUT_FLAGS "${CURRENT_INDEX}" INSTRUCTION_SET_OUT_FLAG)
        string(REPLACE "." ";" INSTRUCTION_SET_OUT_FLAG "${INSTRUCTION_SET_OUT_FLAG}")
        string(REPLACE "." ";" INSTRUCTION_SET_INTRINSIC "${INSTRUCTION_SET_INTRINSIC}")
		jsonifier_check_instruction_set("${INSTRUCTION_SET_NAME}" "${INSTRUCTION_SET_IN_FLAG}" "${INSTRUCTION_SET_OUT_FLAG}" "${INSTRUCTION_SET_INTRINSIC}")
	endforeach()

	message(STATUS "Detected CPU Architecture: ${JSONIFIER_CPU_INSTRUCTIONS}")
else()
	message(STATUS "SSE not supported by architecture ${CMAKE_SYSTEM_PROCESSOR}")
endif()

set(AVX_FLAG)
set(JSONIFIER_CPU_INSTRUCTIONS 0)

if (NOT "${POPCNT}" STREQUAL "")
    list(APPEND AVX_FLAG "${POPCNT}")
    math(EXPR JSONIFIER_CPU_INSTRUCTIONS "${JSONIFIER_CPU_INSTRUCTIONS} | 1 << 0" OUTPUT_FORMAT DECIMAL)
endif()
if (NOT "${LZCNT}" STREQUAL "")
    list(APPEND AVX_FLAG "${LZCNT}")
    math(EXPR JSONIFIER_CPU_INSTRUCTIONS "${JSONIFIER_CPU_INSTRUCTIONS} | 1 << 1" OUTPUT_FORMAT DECIMAL)
endif()
if (NOT "${BMI}" STREQUAL "")
    list(APPEND AVX_FLAG "${BMI}")
    math(EXPR JSONIFIER_CPU_INSTRUCTIONS "${JSONIFIER_CPU_INSTRUCTIONS} | 1 << 2" OUTPUT_FORMAT DECIMAL)
endif()
if (NOT "${AVX512}" STREQUAL "")
    list(APPEND AVX_FLAG "${AVX512}")
    math(EXPR JSONIFIER_CPU_INSTRUCTIONS "${JSONIFIER_CPU_INSTRUCTIONS} | 1 << 5" OUTPUT_FORMAT DECIMAL)
elseif (NOT "${AVX2}" STREQUAL "")
    list(APPEND AVX_FLAG "${AVX2}")
    math(EXPR JSONIFIER_CPU_INSTRUCTIONS "${JSONIFIER_CPU_INSTRUCTIONS} | 1 << 4" OUTPUT_FORMAT DECIMAL)
elseif (NOT "${AVX}" STREQUAL "")
    list(APPEND AVX_FLAG "${AVX}")
    math(EXPR JSONIFIER_CPU_INSTRUCTIONS "${JSONIFIER_CPU_INSTRUCTIONS} | 1 << 3" OUTPUT_FORMAT DECIMAL)
endif()

set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS_SAVE}")