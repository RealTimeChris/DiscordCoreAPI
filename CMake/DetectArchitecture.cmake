#	MIT License
#
#	Copyright (c) 2023 RealTimeChris
#
#	Permission is hereby granted, free of charge, to any person obtaining a copy of this 
#	software and associated documentation files (the "Software"), to deal in the Software 
#	without restriction, including without limitation the rights to use, copy, modify, merge, 
#	publish, distribute, sublicense, and/or sell copies of the Software, and to permit 
#	persons to whom the Software is furnished to do so, subject to the following conditions:
#
#	The above copyright notice and this permission notice shall be included in all copies or 
#	substantial portions of the Software.
#
#	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
#	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
#	PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE 
#	FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR 
#	OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
#	DEALINGS IN THE SOFTWARE.
#
# CMakeLists.txt - The CMake script for building this library.
# Dec 17, 2022
# https://github.com/RealTimeChris/Jsonifier
include(CheckCXXSourceRuns)

function(check_instruction_set INSTRUCTION_SET_NAME INSTRUCTION_SET_FLAG INSTRUCTION_SET_INTRINSIC)

    set(INSTRUCTION_SET_CODE "
        #include <immintrin.h>
        #include <stdint.h>
        int main()
        {
            ${INSTRUCTION_SET_INTRINSIC};
            return 0;
        }
    ")

    set(CMAKE_REQUIRED_FLAGS "${INSTRUCTION_SET_FLAG}")
    CHECK_CXX_SOURCE_RUNS("${INSTRUCTION_SET_CODE}" "${INSTRUCTION_SET_NAME}")
    if(${INSTRUCTION_SET_NAME})
        set(AVX_FLAG "${INSTRUCTION_SET_FLAG}" PARENT_SCOPE)
        set(AVX_NAME "${INSTRUCTION_SET_NAME}" PARENT_SCOPE)
    else()
        message(STATUS "Instruction set ${INSTRUCTION_SET_NAME} not supported. Falling back to the previous instruction set.")
        return()
    endif()
endfunction()

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(INSTRUCTION_SETS
        "T_AVX?/arch:AVX?__m128i value{}#auto result = _mm_extract_epi32(value, 0)"
        "T_AVX2?/arch:AVX2?__m256i value{}#auto result = _mm256_extract_epi32(value, 0)"
        "T_AVX512?/arch:AVX512?int32_t result[16]#const _mm512i& value{}#_mm512_store_si512(result, value)"
    )
else()
    set(INSTRUCTION_SETS
        "T_AVX?-mavx?__m128i value{}#auto result = _mm_extract_epi32(value, 0)"
        "T_AVX2?-mavx2?__m256i value{}#auto result = _mm256_extract_epi32(value, 0)"
        "T_AVX512?-mavx512f?int32_t result[16]#const _mm512i& value{}#_mm512_store_si512(result, value)"
)
endif()

set(CMAKE_REQUIRED_FLAGS_SAVE "${CMAKE_REQUIRED_FLAGS}")

set(AVX_NAME "T_Fallback")

foreach(INSTRUCTION_SET IN LISTS INSTRUCTION_SETS)
    string(REPLACE "?" ";" CURRENT_LIST "${INSTRUCTION_SET}")
    list(GET CURRENT_LIST 0 INSTRUCTION_SET_NAME)
    list(GET CURRENT_LIST 1 INSTRUCTION_SET_FLAG)
    string(REPLACE "." ";" INSTRUCTION_SET_FLAG "${INSTRUCTION_SET_FLAG}")
    list(GET CURRENT_LIST 2 INSTRUCTION_SET_INTRINSIC)
    string(REPLACE "#" ";" INSTRUCTION_SET_INTRINSIC "${INSTRUCTION_SET_INTRINSIC}")
    check_instruction_set("${INSTRUCTION_SET_NAME}" "${INSTRUCTION_SET_FLAG}" "${INSTRUCTION_SET_INTRINSIC}")
endforeach()

message(STATUS "Detected CPU Architecture: ${AVX_NAME}")
set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS_SAVE}")