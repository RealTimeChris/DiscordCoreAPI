#
#	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.
#
#	Copyright 2021, 2022, 2023 Chris M. (RealTimeChris)
#
#	This library is free software; you can redistribute it and/or
#	modify it under the terms of the GNU Lesser General Public
#	License as published by the Free Software Foundation; either
#	version 2.1 of the License, or (at your option) any later version.
#
#	This library is distributed in the hope that it will be useful,
#	but WITHOUT ANY WARRANTY; without even the implied warranty of
#	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#	Lesser General Public License for more details.
#
#	You should have received a copy of the GNU Lesser General Public
#	License along with this library; if not, write to the Free Software
#	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
#	USA
#
# CMakeLists.txt - The CMake script for building this library.
# May 13, 2021
# https://discordcoreapi.com

include(GenerateProductVersion)
generate_product_version(
	PRODUCT_INFO
	NAME "${LIB_NAME}"
	FILE_DESCRIPTION "${DESCRIPTION}"
	ORIGINAL_FILENAME "discordcoreapi.dll"
	COMPANY_NAME "RealTimeChris Inc."
	VERSION_MAJOR "${VERSION_MAJOR}"
	VERSION_MINOR "${VERSION_MINOR}"
	VERSION_PATCH "${VERSION_PATCH}"
)

set(DISCORDCOREAPI-POPCNT_EXITCODE 0 CACHE INTERNAL "For cross-compiling.")
set(DISCORDCOREAPI-LZCNT_EXITCODE 0 CACHE INTERNAL "For cross-compiling.")
set(DISCORDCOREAPI-BMI_EXITCODE 0 CACHE INTERNAL "For cross-compiling.")
set(DISCORDCOREAPI-AVX_EXITCODE 0 CACHE INTERNAL "For cross-compiling.")
set(DISCORDCOREAPI-AVX2_EXITCODE 0 CACHE INTERNAL "For cross-compiling.")
set(DISCORDCOREAPI-AVX512_EXITCODE 0 CACHE INTERNAL "For cross-compiling.")

set(CMAKE_CXX_EXTENSIONS OFF)

if (NOT DEFINED BUILD_SHARED_LIBS)
	set(BUILD_SHARED_LIBS TRUE)
endif()

file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/../Include/discordcoreapi/*.hpp" "${CMAKE_CURRENT_SOURCE_DIR}/../Include/discordcoreapi/Utilities/*.hpp")
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/../Source/*.cpp")

if (WIN32)
	if (BUILD_SHARED_LIBS)
		add_library("${LIB_NAME}" SHARED "${SOURCES}" "${PRODUCT_INFO}")
	else()
		add_library("${LIB_NAME}" STATIC "${SOURCES}" "${PRODUCT_INFO}")
	endif()
elseif (APPLE)
	if (BUILD_SHARED_LIBS)
		add_library("${LIB_NAME}" SHARED "${SOURCES}")
	else()
		add_library("${LIB_NAME}" STATIC "${SOURCES}")
	endif()
	find_package(Threads REQUIRED)
elseif (UNIX)
	if (BUILD_SHARED_LIBS)
		add_library("${LIB_NAME}" SHARED "${SOURCES}")
	else()
		add_library("${LIB_NAME}" STATIC "${SOURCES}")
	endif()
	find_package(Threads REQUIRED)
endif()

add_library("${PROJECT_NAME}::${LIB_NAME}" ALIAS "${LIB_NAME}")

set_target_properties(
	"${LIB_NAME}" PROPERTIES 
	OUTPUT_NAME "discordcoreapi"
)

if(DEV)
	include(FetchContent)
	FetchContent_Declare(
	   Jsonifier
	   GIT_REPOSITORY https://github.com/RealTimeChris/Jsonifier.git
	   GIT_TAG dev
	)
	FetchContent_MakeAvailable(Jsonifier)
else()
	find_package(Jsonifier CONFIG REQUIRED)
endif()

find_package(OpenSSL REQUIRED)
find_package(Opus CONFIG REQUIRED)
find_package(unofficial-sodium CONFIG REQUIRED)

if(NOT DEFINED JSONIFIER_CPU_INSTRUCTIONS)
	include("DCADetectArchitecture")
endif()

set(JSONIFIER_CPU_INSTRUCTIONS "${JSONIFIER_CPU_INSTRUCTIONS}" CACHE INTERNAL "For the CPU architecture selection.")
set(AVX_FLAG "${AVX_FLAG}" CACHE INTERNAL "For the CPU flag selection.")

target_include_directories(
	"${LIB_NAME}" PUBLIC
	"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../Include>"
	"$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>"
)

target_link_libraries(
	"${LIB_NAME}" PUBLIC
	"$<$<TARGET_EXISTS:unofficial-sodium::sodium>:unofficial-sodium::sodium>"
	"$<$<TARGET_EXISTS:Jsonifier::Jsonifier>:Jsonifier::Jsonifier>"
	"$<$<TARGET_EXISTS:Threads::Threads>:Threads::Threads>"
	"$<$<TARGET_EXISTS:OpenSSL::Crypto>:OpenSSL::Crypto>"
	"$<$<TARGET_EXISTS:OpenSSL::SSL>:OpenSSL::SSL>"
	"$<$<TARGET_EXISTS:Opus::opus>:Opus::opus>"
)

target_compile_definitions(
	"${LIB_NAME}" PUBLIC 
	"$<$<NOT:$<BOOL:${BUILD_SHARED_LIBS}>>:DiscordCoreAPI_EXPORTS_NOPE>"
	"JSONIFIER_CPU_INSTRUCTIONS=${JSONIFIER_CPU_INSTRUCTIONS}"
)

include(ProcessorCount)
ProcessorCount(N)
MATH(EXPR THREAD_COUNT "${N} / 3")

target_compile_options(
	"${LIB_NAME}" PUBLIC
	"$<$<CXX_COMPILER_ID:MSVC>:$<$<STREQUAL:${ASAN_ENABLED},TRUE>:/fsanitize=address>>"
	"$<$<CXX_COMPILER_ID:MSVC>:/MP${THREAD_COUNT}>"
	"$<$<CXX_COMPILER_ID:CLANG>:-fcoroutines>"
	"$<$<CXX_COMPILER_ID:GNU>:-fcoroutines>"
	"$<$<CXX_COMPILER_ID:CLANG>:-Wextra>"
	"$<$<CXX_COMPILER_ID:MSVC>:/bigobj>"
	"$<$<CXX_COMPILER_ID:CLANG>:-Wall>"
	"$<$<CXX_COMPILER_ID:GNU>:-Wextra>"
	"$<$<CXX_COMPILER_ID:MSVC>:/Wall>"
	"$<$<CXX_COMPILER_ID:MSVC>:/EHsc>"
	"$<$<CXX_COMPILER_ID:GNU>:-Wall>"
	"$<$<CXX_COMPILER_ID:MSVC>:/Zi>"
	"${AVX_FLAG}"
)

target_link_options(
	"${LIB_NAME}" PUBLIC
	"$<$<CXX_COMPILER_ID:GNU>:$<$<STREQUAL:${ASAN_ENABLED},TRUE>:-fsanitize=address>>"
	"$<$<CXX_COMPILER_ID:MSVC>:/DEBUG>"
)

set(CONFIG_FILE_NAME "${PROJECT_NAME}Config.cmake")
set(EXPORTED_TARGETS_NAME "${PROJECT_NAME}Targets")
set(EXPORTED_TARGETS_FILE_NAME "${EXPORTED_TARGETS_NAME}.cmake")
set(EXPORTED_TARGETS_FILE_PATH "share/discordcoreapi/${EXPORTED_TARGETS_FILE_NAME}")
set(VERSION_FILE_NAME "${PROJECT_NAME}ConfigVersion.cmake")
set(RELEASE_PDB_FILE_PATH "bin/discordcoreapi.pdb")
set(DEBUG_PDB_FILE_PATH "debug/bin/discordcoreapi.pdb")

include(CMakePackageConfigHelpers)
configure_package_config_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/../CMake/${CONFIG_FILE_NAME}.in"
	"${CMAKE_CURRENT_BINARY_DIR}/${CONFIG_FILE_NAME}"
	INSTALL_DESTINATION "share/discordcoreapi"
	PATH_VARS
	EXPORTED_TARGETS_FILE_PATH
	RELEASE_PDB_FILE_PATH
	DEBUG_PDB_FILE_PATH
)

write_basic_package_version_file(
	"${CMAKE_CURRENT_BINARY_DIR}/${VERSION_FILE_NAME}"
	VERSION "${PRODUCT_VERSION}"
	COMPATIBILITY AnyNewerVersion
)

install(
	FILES
	"${CMAKE_CURRENT_BINARY_DIR}/${CONFIG_FILE_NAME}"
	"${CMAKE_CURRENT_BINARY_DIR}/${VERSION_FILE_NAME}"
	DESTINATION "share/discordcoreapi"
)

install(
	DIRECTORY
	"${CMAKE_CURRENT_SOURCE_DIR}/../Include/"
	DESTINATION "include"
)

install(
	TARGETS "${LIB_NAME}"
	EXPORT "${EXPORTED_TARGETS_NAME}"
	RUNTIME DESTINATION "$<$<NOT:$<PLATFORM_ID:Linux>>:$<IF:$<CONFIG:Debug>,bin,bin>>"
	ARCHIVE DESTINATION "$<IF:$<CONFIG:Debug>,lib,lib>"
)

install(
	EXPORT "${EXPORTED_TARGETS_NAME}"
	FILE "${EXPORTED_TARGETS_FILE_NAME}"
	NAMESPACE "${PROJECT_NAME}::"
	DESTINATION "share/discordcoreapi"
)

if (WIN32 AND "${BUILD_SHARED_LIBS}")
	install(
		FILES 
		"$<TARGET_PDB_FILE:${LIB_NAME}>"
		DESTINATION "$<IF:$<CONFIG:Debug>,bin,bin>"
		OPTIONAL
	)
endif()