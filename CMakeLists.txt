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
set(VCPKG_ROOT_DIR "C:/Vcpkg")
set(Opus_DIR "C:/Vcpkg/installed/x64-windows/share/opus") # Set this one to the folder location of the file "OpusConfig.cmake".
set(unofficial-sodium_DIR "C:/Vcpkg/installed/x64-windows/share/unofficial-sodium") # Set this one to the folder location of the file "unofficial-sodiumConfig.cmake".
set(Jsonifier_DIR "C:/vcpkg2/installed/x64-windows/share/jsonifier/") # Set this one to the folder location of the file "JsonifierConfig.cmake".
set(OPENSSL_ROOT_DIR "C:/Vcpkg/installed/x64-windows/") # Set this one to the folder location of the include folder and library folders of OpenSSL.

if (EXISTS "${VCPKG_ROOT_DIR}")
	set(ENV{VCPKG_INSTALLATION_ROOT} "${VCPKG_ROOT_DIR}")
endif()

cmake_minimum_required(VERSION 3.20)
if (WIN32)
	set(OS "windows")
elseif(UNIX)
	set(OS "linux")
elseif(APPLE)
	set(OS "osx")
endif()
if (NOT "$ENV{VCPKG_INSTALLATION_ROOT}" STREQUAL "")
	set(VCPKG_INSTALLATION_ROOT_NEW "$ENV{VCPKG_INSTALLATION_ROOT}")
	set(OPENSSL_ROOT_DIR "${VCPKG_INSTALLATION_ROOT_NEW}/installed/x64-${OS}/")
	set(OpenSSL_DIR "${VCPKG_INSTALLATION_ROOT_NEW}/installed/x64-${OS}/share/openssl")
	set(Jsonifier_DIR "${VCPKG_INSTALLATION_ROOT_NEW}/installed/x64-${OS}/share/jsonifier")
	set(Opus_DIR "${VCPKG_INSTALLATION_ROOT_NEW}/installed/x64-${OS}/share/opus")
	set(unofficial-sodium_DIR "${VCPKG_INSTALLATION_ROOT_NEW}/installed/x64-${OS}/share/unofficial-sodium")
	include("${VCPKG_INSTALLATION_ROOT_NEW}/scripts/buildsystems/vcpkg.cmake")
	set(OS_NAME "${CMAKE_SYSTEM_NAME}")
	string(TOLOWER "${OS_NAME}" OS_NAME)
	set(DEBUG_PREFIX "")
else()
	message(WARNING "No Vcpkg root folder found, Please make sure that you properly set the library folders.")
	set(VCPKG_INSTALLATION_ROOT_NEW "${VCPKG_ROOT_DIR}")
endif()

set(PROJECT_NAME "DiscordCoreAPI")
set(LIB_NAME "${PROJECT_NAME}")
set(PRODUCT_VERSION "${VERSION}")
set(DESCRIPTION "A Discord bot library, written in C++, using custom asynchronous coroutines.")

project(
	"${PROJECT_NAME}"
	VERSION "${PRODUCT_VERSION}"
	LANGUAGES CXX
	HOMEPAGE_URL "https://discordcoreapi.com"
	DESCRIPTION "${DESCRIPTION}"
)
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/CMake;")
set(CMAKE_CONFIGURATION_TYPES "${CMAKE_BUILD_TYPE}")

add_subdirectory(Library)