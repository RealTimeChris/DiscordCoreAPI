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

# Set these values, if you are not building with VCPKG.
# Set this value...
set(VCPKG_ROOT_DIR "")
# OR this value...
set(CMAKE_CONFIG_FILE_DIR "C:/USers/Chris/Cmake/DiscordCoreAPI/share/discordcoreapi")
# AND these values.
set(ffmpeg_DIR "C:/vcpkg/installed/x64-windows/share/ffmpeg")
set(nlohmann_json_DIR "C:/vcpkg/installed/x64-windows/share/nlohmann_json")
set(Opus_DIR "C:/Vcpkg/installed/x64-windows/share/opus")
set(unofficial-sodium_DIR "C:/vcpkg/installed/x64-windows/share/unofficial-sodium")
set(OPENSSL_ROOT_DIR "C:/vcpkg/installed/x64-windows/")
set(simdjson_DIR "C:/vcpkg/installed/x64-windows/share/simdjson")

cmake_minimum_required(VERSION 3.20)
set(PROJECT_NAME "DiscordCoreAPI")

set(LIB_NAME "${PROJECT_NAME}")
set(PRODUCT_VERSION "1.0.0")
set(DESCRIPTION "A Discord bot library, written in C++, using custom asynchronous coroutines.")
project(
	"${PROJECT_NAME}"
	VERSION "${PRODUCT_VERSION}"
	LANGUAGES CXX
	HOMEPAGE_URL "https://discordcoreapi.com"
	DESCRIPTION "${DESCRIPTION}"
)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake;C:/vcpkg/installed/x64-windows/share/ffmpeg")
set(CMAKE_CONFIGURATION_TYPES "${CMAKE_BUILD_TYPE}")

if (NOT EXISTS "${_VCPKG_INSTALLED_DIR}")
	set(DEBUG_PREFIX "debug/")
	if (WIN32)
		if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
			cmake_path(GET CMAKE_ROOT ROOT_PATH ROOT_DRIVE)
			set(CMAKE_INSTALL_PREFIX "${ROOT_DRIVE}Users/$ENV{USERNAME}/CMake/${PROJECT_NAME}" CACHE PATH "The CMake install prefix." FORCE)
		endif()
	elseif(UNIX)
		if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
			set(CMAKE_INSTALL_PREFIX "/home/$ENV{USERNAME}/CMake/${PROJECT_NAME}" CACHE PATH "The CMake install prefix." FORCE)
		endif()
	else()
		message(FATAL_ERROR "Sorry, but your OS is not supported!")
	endif()
else()
	set(DEBUG_PREFIX "")
	cmake_path(GET _VCPKG_INSTALLED_DIR ROOT_PATH ROOT_DRIVE)
	set(VCPKG_INSTALLED_DIR "${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}")
	cmake_path(ABSOLUTE_PATH VCPKG_INSTALLED_DIR BASE_DIRECTORY "${ROOT_DRIVE}" NORMALIZE OUTPUT_VARIABLE VCPKG_INSTALLED_DIR)
	set(VCPKG_ROOT_DIR "${VCPKG_INSTALLED_DIR}../")
endif()

if (EXISTS "${VCPKG_ROOT_DIR}")
	include("${VCPKG_ROOT_DIR}/scripts/buildsystems/vcpkg.cmake")
	set(VCPKG_INSTALLED_DIR "${VCPKG_ROOT_DIR}/installed/x64-windows")
	set(VCPKG_INCLUDE_DIR "${VCPKG_INSTALLED_DIR}/include")
endif()

add_subdirectory(Library)