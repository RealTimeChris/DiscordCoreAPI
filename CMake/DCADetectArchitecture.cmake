# JsonifierDetectArchitecture.cmake - Script for detecting the CPU architecture.
# MIT License
# Copyright (c) 2023 RealTimeChris
# https://discordcoreapi.com

if (UNIX OR APPLE)
    file(WRITE "${CMAKE_CURRENT_SOURCE_DIR}/../CMake/BuildFeatureTester.sh" "#!/bin/bash
\"${CMAKE_COMMAND}\" -S ./ -B ./Build -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
\"${CMAKE_COMMAND}\" --build ./Build --config=Release")
    execute_process(
        COMMAND chmod +x "${CMAKE_CURRENT_SOURCE_DIR}/../CMake/BuildFeatureTester.sh"
        RESULT_VARIABLE CHMOD_RESULT
    )
    if(NOT ${CHMOD_RESULT} EQUAL 0)
        message(FATAL_ERROR "Failed to set executable permissions for BuildFeatureTester.sh")
    endif()
    execute_process(
        COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/../CMake/BuildFeatureTester.sh"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../CMake"
    )
    set(FEATURE_TESTER_FILE "${CMAKE_CURRENT_SOURCE_DIR}/../CMake/Build/feature_detector")
elseif(WIN32)
    file(WRITE "${CMAKE_CURRENT_SOURCE_DIR}/../CMake/BuildFeatureTester.bat" "\"${CMAKE_COMMAND}\" -S ./ -B ./Build -DCMAKE_BUILD_TYPE=Release
\"${CMAKE_COMMAND}\" --build ./Build --config=Release")
    execute_process(
        COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/../CMake/BuildFeatureTester.bat"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../CMake"
    )
    set(FEATURE_TESTER_FILE "${CMAKE_CURRENT_SOURCE_DIR}/../CMake/Build/Release/feature_detector.exe")
endif()

execute_process(
    COMMAND "${FEATURE_TESTER_FILE}"
    RESULT_VARIABLE DCA_CPU_INSTRUCTIONS_NEW
)

function(is_numeric value result_var)
    string(REGEX MATCH "^[0-9]+(\\.[0-9]+)?$" is_numeric_match "${value}")
    if(is_numeric_match)
        set(${result_var} TRUE PARENT_SCOPE)
    else()
        set(${result_var} FALSE PARENT_SCOPE)
    endif()
endfunction()

is_numeric("${DCA_CPU_INSTRUCTIONS_NEW}" is_numeric_result)

set(AVX_FLAG "")
if (is_numeric_result)
math(EXPR DCA_CPU_INSTRUCTIONS_NUMERIC "${DCA_CPU_INSTRUCTIONS_NEW}")
math(EXPR DCA_CPU_INSTRUCTIONS 0)

function(check_instruction_set INSTRUCTION_SET_NAME INSTRUCTION_SET_FLAG INSTRUCTION_SET_NUMERIC_VALUE)
    math(EXPR INSTRUCTION_PRESENT "( ${DCA_CPU_INSTRUCTIONS_NUMERIC} & ${INSTRUCTION_SET_NUMERIC_VALUE} )")
    if(INSTRUCTION_PRESENT)
        message(STATUS "Instruction Set Found: ${INSTRUCTION_SET_NAME}")
        math(EXPR DCA_CPU_INSTRUCTIONS "( ${DCA_CPU_INSTRUCTIONS} | ${INSTRUCTION_SET_NUMERIC_VALUE} )")
        set(AVX_FLAG "${AVX_FLAG};${INSTRUCTION_SET_FLAG}" PARENT_SCOPE)
    endif()
endfunction()

math(EXPR INSTRUCTION_PRESENT "( ${DCA_CPU_INSTRUCTIONS_NUMERIC} & 0x1 )")
if(INSTRUCTION_PRESENT)
    math(EXPR DCA_CPU_INSTRUCTIONS "${DCA_CPU_INSTRUCTIONS} | 1 << 0" OUTPUT_FORMAT DECIMAL)
endif()
math(EXPR INSTRUCTION_PRESENT "( ${DCA_CPU_INSTRUCTIONS_NUMERIC} & 0x2 )")
if(INSTRUCTION_PRESENT)
    math(EXPR DCA_CPU_INSTRUCTIONS "${DCA_CPU_INSTRUCTIONS} | 1 << 1" OUTPUT_FORMAT DECIMAL)
endif()
math(EXPR INSTRUCTION_PRESENT "( ${DCA_CPU_INSTRUCTIONS_NUMERIC} & 0x4 )")
if(INSTRUCTION_PRESENT)
    math(EXPR DCA_CPU_INSTRUCTIONS "${DCA_CPU_INSTRUCTIONS} | 1 << 2" OUTPUT_FORMAT DECIMAL)
endif()
math(EXPR INSTRUCTION_PRESENT "( ${DCA_CPU_INSTRUCTIONS_NUMERIC} & 0x8 )")
if(INSTRUCTION_PRESENT)
    math(EXPR DCA_CPU_INSTRUCTIONS "${DCA_CPU_INSTRUCTIONS} | 1 << 3" OUTPUT_FORMAT DECIMAL)
endif()

math(EXPR INSTRUCTION_PRESENT "( ${DCA_CPU_INSTRUCTIONS_NUMERIC} & 0x10 )")
if(INSTRUCTION_PRESENT)
    math(EXPR DCA_CPU_INSTRUCTIONS "${DCA_CPU_INSTRUCTIONS} | 1 << 4" OUTPUT_FORMAT DECIMAL)
endif()
math(EXPR INSTRUCTION_PRESENT128 "( ${DCA_CPU_INSTRUCTIONS_NUMERIC} & 0x20 )")
math(EXPR INSTRUCTION_PRESENT256 "( ${DCA_CPU_INSTRUCTIONS_NUMERIC} & 0x40 )")
math(EXPR INSTRUCTION_PRESENT512 "( ${DCA_CPU_INSTRUCTIONS_NUMERIC} & 0x80 )")
if(INSTRUCTION_PRESENT512)
    math(EXPR DCA_CPU_INSTRUCTIONS "${DCA_CPU_INSTRUCTIONS} | 1 << 7" OUTPUT_FORMAT DECIMAL)
elseif(INSTRUCTION_PRESENT256)
    math(EXPR DCA_CPU_INSTRUCTIONS "${DCA_CPU_INSTRUCTIONS} | 1 << 6" OUTPUT_FORMAT DECIMAL)
elseif(INSTRUCTION_PRESENT128)
    math(EXPR DCA_CPU_INSTRUCTIONS "${DCA_CPU_INSTRUCTIONS} | 1 << 5" OUTPUT_FORMAT DECIMAL)
endif() 

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    check_instruction_set("LzCnt" "" 0x1)
    check_instruction_set("PopCnt" "" 0x2)
    check_instruction_set("Bmi" "" 0x4)
    check_instruction_set("Bmi2" "" 0x8)
    check_instruction_set("Neon" "" 0x10)
    check_instruction_set("Avx" "/arch:AVX" 0x20)
    check_instruction_set("Avx2" "/arch:AVX2" 0x40)
    check_instruction_set("Avx512" "/arch:AVX512" 0x80)
else()
    check_instruction_set("LzCnt" "-mlzcnt" 0x1)
    check_instruction_set("PopCnt" "-mpopcnt" 0x2)
    check_instruction_set("Bmi" "-mbmi" 0x4)
    check_instruction_set("Bmi2" "-mbmi2" 0x8)
    check_instruction_set("Neon" "" 0x10)
    check_instruction_set("Avx" "-mavx;-mlzcnt;-mpopcnt;-mbmi;-mbmi2" 0x20)
    check_instruction_set("Avx2" "-mavx2;-mavx;-mlzcnt;-mpopcnt;-mbmi;-mbmi2" 0x40)
    check_instruction_set("Avx512" "-mavx512f;-mavx2;-mavx;-mlzcnt;-mpopcnt;-mbmi;-mbmi2" 0x80)
endif()
else()
set(DCA_CPU_INSTRUCTIONS "0")
endif()
set(AVX_FLAG "${AVX_FLAG}" CACHE STRING "AVX flags" FORCE)
set(DCA_CPU_INSTRUCTIONS "${DCA_CPU_INSTRUCTIONS}" CACHE STRING "CPU Instruction Sets" FORCE)
