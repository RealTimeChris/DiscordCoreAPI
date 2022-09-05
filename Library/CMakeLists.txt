#
#	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.
#
#	Copyright 2021, 2022 Chris M. (RealTimeChris)
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
	VERSION_MAJOR "1"
	VERSION_MINOR "0"
	VERSION_PATCH "0"
	VERSION_REVISION "0"
)

file(GLOB SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/Source/*.cpp")

if (WIN32)
	add_library("${LIB_NAME}" SHARED "${SOURCES}" "${PRODUCT_INFO}")
elseif (UNIX)
	add_library("${LIB_NAME}" STATIC "${SOURCES}")
	find_package(Threads REQUIRED)
endif()

add_library("${PROJECT_NAME}::${LIB_NAME}" ALIAS "${LIB_NAME}")

file(GLOB HEADERS CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/Include/discordcoreapi/*.hpp")

set_target_properties(
	"${LIB_NAME}" PROPERTIES
	PUBLIC_HEADER "${HEADERS}"
	OUTPUT_NAME "discordcoreapi"
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF
)

target_include_directories(
	"${LIB_NAME}" PUBLIC
	"$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>"
	PRIVATE
	"$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Include/>"
)

find_package(FFMPEG MODULE REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Opus CONFIG REQUIRED)
find_package(unofficial-sodium CONFIG REQUIRED)
find_package(simdjson CONFIG REQUIRED)

target_link_libraries(
	"${LIB_NAME}" PUBLIC
	$<$<TARGET_EXISTS:ffmpeg::libavformat>:ffmpeg::libavformat>
	$<$<TARGET_EXISTS:ffmpeg::libavcodec>:ffmpeg::libavcodec>
	$<$<TARGET_EXISTS:ffmpeg::libavutil>:ffmpeg::libavutil>
	$<$<TARGET_EXISTS:ffmpeg::libswresample>:ffmpeg::libswresample>
	$<$<TARGET_EXISTS:nlohmann_json::nlohmann_json>:nlohmann_json::nlohmann_json>
	$<$<TARGET_EXISTS:OpenSSL::Crypto>:OpenSSL::Crypto>
	$<$<TARGET_EXISTS:OpenSSL::SSL>:OpenSSL::SSL>
	$<$<TARGET_EXISTS:Opus::opus>:Opus::opus>
	$<$<TARGET_EXISTS:simdjson::simdjson>:simdjson::simdjson>
	$<$<TARGET_EXISTS:unofficial-sodium::sodium>:unofficial-sodium::sodium>
	$<$<TARGET_EXISTS:Threads::Threads>:Threads::Threads>
)

target_compile_features(
	"${LIB_NAME}" PUBLIC
	"cxx_std_20"
)

if (DEFINED ENV{EnableASAN})
	if(MSVC)
		target_compile_options("${LIB_NAME}" PUBLIC "/fsanitize=address" "/Zi" "/Debug")
		target_link_options("${LIB_NAME}" PUBLIC "/incremental:no" "/Debug")	
	else()
		target_compile_options("${LIB_NAME}" PUBLIC "-fsanitize=address")
		target_link_options("${LIB_NAME}" PUBLIC "-fsanitize=address")
	endif()
endif()

if ("${VCPKG_TARGET_TRIPLET}" STREQUAL "x64-uwp")
	target_compile_definitions("${LIB_NAME}" PUBLIC "UWP")
endif()

target_compile_options(
	"${LIB_NAME}" PRIVATE
	"$<$<NOT:$<PLATFORM_ID:Linux>>:/bigobj>"
	"$<$<PLATFORM_ID:Windows>:/MP>"
	"$<$<PLATFORM_ID:Linux>:-O3>"
)

target_link_options(
	"${LIB_NAME}" PUBLIC
	"$<$<PLATFORM_ID:Windows>:/DEBUG>"
)

set(CONFIG_FILE_NAME "${PROJECT_NAME}Config.cmake")
set(EXPORTED_TARGETS_NAME "${PROJECT_NAME}Targets")
set(EXPORTED_TARGETS_FILE_NAME "${EXPORTED_TARGETS_NAME}.cmake")
set(EXPORTED_TARGETS_FILE_PATH "share/discordcoreapi/${EXPORTED_TARGETS_FILE_NAME}")
set(RELEASE_PDB_FILE_PATH "bin/discordcoreapi.pdb")
set(DEBUG_PDB_FILE_PATH "debug/bin/discordcoreapi.pdb")

include(CMakePackageConfigHelpers)
configure_package_config_file(
	"${CMAKE_SOURCE_DIR}/CMake/${CONFIG_FILE_NAME}.in"
	"${CMAKE_CURRENT_BINARY_DIR}/${CONFIG_FILE_NAME}"
	INSTALL_DESTINATION "share/discordcoreapi"
	PATH_VARS
	EXPORTED_TARGETS_FILE_PATH
	RELEASE_PDB_FILE_PATH
	DEBUG_PDB_FILE_PATH
)

set(VERSION_FILE_NAME "${PROJECT_NAME}ConfigVersion.cmake")

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
	TARGETS "${LIB_NAME}"
	EXPORT "${EXPORTED_TARGETS_NAME}"
	RUNTIME DESTINATION "$<$<NOT:$<PLATFORM_ID:Linux>>:$<IF:$<CONFIG:Debug>,${DEBUG_PREFIX}bin,bin>>"
	ARCHIVE DESTINATION "$<IF:$<CONFIG:Debug>,${DEBUG_PREFIX}lib,lib>"
	PUBLIC_HEADER DESTINATION "include/discordcoreapi"
)

install(
	EXPORT "${EXPORTED_TARGETS_NAME}"
	FILE "${EXPORTED_TARGETS_FILE_NAME}"
	NAMESPACE "${PROJECT_NAME}::"
	DESTINATION "share/discordcoreapi"
)

if (WIN32)
	install(
		FILES 
		"$<TARGET_PDB_FILE:${LIB_NAME}>"
		DESTINATION "$<IF:$<CONFIG:Debug>,${DEBUG_PREFIX}bin,bin>"
		OPTIONAL
	)
endif()
