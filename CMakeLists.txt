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

# Set this value...
set(_VCPKG_ROOT_DIR "")
# OR these values.
set(FFMPEG_DIR "C:/Vcpkg/installed/x64-windows/share/ffmpeg") # Set this one to the folder location of the file "FindFFMPEG.cmake".
set(Opus_DIR "C:/Vcpkg/installed/x64-windows/share/opus") # Set this one to the folder location of the file "OpusConfig.cmake".
set(simdjson_DIR "C:/Vcpkg/installed/x64-windows/share/simdjson") # Set this one to the folder location of the file "simdjson-config.cmake".
set(unofficial-sodium_DIR "C:/Vcpkg/installed/x64-windows/share/unofficial-sodium") # Set this one to the folder location of the file "unofficial-sodiumConfig.cmake".
set(OPENSSL_ROOT_DIR "C:/Vcpkg/installed/x64-windows/") # Set this one to the folder location of the include folder and library folders of OpenSSL.

cmake_minimum_required(VERSION 3.20)
set(PROJECT_NAME "DiscordCoreAPI")
set(LIB_NAME "${PROJECT_NAME}")
set(PRODUCT_VERSION "1.2.0")
set(DESCRIPTION "A Discord bot library, written in C++, using custom asynchronous coroutines.")

project(
	"${PROJECT_NAME}"
	VERSION "${PRODUCT_VERSION}"
	LANGUAGES CXX
	HOMEPAGE_URL "https://discordcoreapi.com"
	DESCRIPTION "${DESCRIPTION}"
)
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_SOURCE_DIR}/CMake;${FFMPEG_DIR};${OpenSSL_DIR}")
set(CMAKE_CONFIGURATION_TYPES "${CMAKE_BUILD_TYPE}")

if(EXISTS "${_VCPKG_ROOT_DIR}")
	include("${_VCPKG_ROOT_DIR}/scripts/buildsystems/vcpkg.cmake")
	set(OS_NAME "${CMAKE_SYSTEM_NAME}")
	string(TOLOWER "${OS_NAME}" OS_NAME)
	set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${_VCPKG_ROOT_DIR}/installed/x64-${OS_NAME}/share/ffmpeg")
endif()
if (NOT EXISTS "${_VCPKG_INSTALLED_DIR}")
	set(DEBUG_PREFIX "debug/")
	if (WIN32)
		if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
			cmake_path(GET CMAKE_ROOT ROOT_PATH ROOT_DRIVE)
			if (DEFINED ENV{EnableASAN})
				set(CMAKE_INSTALL_PREFIX "${ROOT_DRIVE}Users/$ENV{USERNAME}/CMake/${PROJECT_NAME}/ASAN" CACHE PATH "The CMake install prefix." FORCE)
			else()
				set(CMAKE_INSTALL_PREFIX "${ROOT_DRIVE}Users/$ENV{USERNAME}/CMake/${PROJECT_NAME}" CACHE PATH "The CMake install prefix." FORCE)
			endif()
		endif()
	elseif(UNIX)
		if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
			if (DEFINED ENV{EnableASAN})
				set(CMAKE_INSTALL_PREFIX "/home/$ENV{USERNAME}/CMake/${PROJECT_NAME}/ASAN" CACHE PATH "The CMake install prefix." FORCE)
			else()
				set(CMAKE_INSTALL_PREFIX "/home/$ENV{USERNAME}/CMake/${PROJECT_NAME}" CACHE PATH "The CMake install prefix." FORCE)
			endif()
		endif()
	else()
		message(FATAL_ERROR "Sorry, but your OS is not supported!")
	endif()
else()
	set(DEBUG_PREFIX "")
	cmake_path(GET _VCPKG_INSTALLED_DIR ROOT_PATH ROOT_DRIVE)
	set(VCPKG_INSTALLED_DIR "${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}")
	cmake_path(ABSOLUTE_PATH VCPKG_INSTALLED_DIR BASE_DIRECTORY "${ROOT_DRIVE}" NORMALIZE OUTPUT_VARIABLE VCPKG_INSTALLED_DIR)
	set(VCPKG_INCLUDE_DIR "${VCPKG_INSTALLED_DIR}/include")
	set(VCPKG_RELEASE_ROOT "${VCPKG_INSTALLED_DIR}/lib")
	set(VCPKG_DEBUG_ROOT "${VCPKG_INSTALLED_DIR}/debug/lib")
endif()

add_subdirectory(Library)