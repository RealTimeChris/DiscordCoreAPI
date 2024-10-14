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
if (EXISTS "${_VCPKG_ROOT_DIR}")
	set(VCPKG_ROOT_DIR "${_VCPKG_ROOT_DIR}")
	set(ENV{VCPKG_INSTALLATION_ROOT} "${_VCPKG_ROOT_DIR}")
else()
	set(VCPKG_ROOT_DIR "C:/Vcpkg")
	set(ENV{VCPKG_INSTALLATION_ROOT} "${VCPKG_ROOT_DIR}")
endif() 

cmake_minimum_required(VERSION 3.20)
if (WIN32)
	if (NOT BUILD_SHARED_LIBS)
		set(OS "windows-static")
	else()
		set(OS "windows")
	endif()
elseif(APPLE)
	if (NOT BUILD_SHARED_LIBS)
		set(OS "osx-static")
	else()
		set(OS "osx")
	endif()
elseif(UNIX)
	if (NOT BUILD_SHARED_LIBS)
		set(OS "linux-static")
	else()
		set(OS "linux")
	endif()
endif()
if (EXISTS "$ENV{VCPKG_INSTALLATION_ROOT}")
	set(VCPKG_INSTALLATION_ROOT_NEW "$ENV{VCPKG_INSTALLATION_ROOT}")
	if (NOT EXISTS "${OPENSSL_ROOT_DIR}")
		set(OPENSSL_ROOT_DIR "${VCPKG_INSTALLATION_ROOT_NEW}/installed/x64-${OS}/")
	endif()
	if (NOT EXISTS "${OpenSSL_DIR}")
		set(OpenSSL_DIR "${VCPKG_INSTALLATION_ROOT_NEW}/installed/x64-${OS}/share/openssl")
	endif()
	if (NOT EXISTS "${Jsonifier_DIR}")
		set(Jsonifier_DIR "${VCPKG_INSTALLATION_ROOT_NEW}/installed/x64-${OS}/share/jsonifier")
	endif()
	if (NOT EXISTS "${Opus_DIR}")
		set(Opus_DIR "${VCPKG_INSTALLATION_ROOT_NEW}/installed/x64-${OS}/share/opus")
	endif()
	if (NOT EXISTS "${unofficial-sodium_DIR}")
		set(unofficial-sodium_DIR "${VCPKG_INSTALLATION_ROOT_NEW}/installed/x64-${OS}/share/unofficial-sodium")
	endif()
	include("${VCPKG_INSTALLATION_ROOT_NEW}/scripts/buildsystems/vcpkg.cmake")
else()
	message(WARNING "No Vcpkg root folder found, Please make sure that you properly set the library folders.")
endif()

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/CMake;")

include(CollectVersion)
collect_version("discordcoreapi")

set(PROJECT_NAME "DiscordCoreAPI")
set(LIB_NAME "${PROJECT_NAME}")
set(DESCRIPTION "A Discord bot library, written in C++, using custom asynchronous coroutines.")

project(
	"${PROJECT_NAME}"
	VERSION "${PRODUCT_VERSION}"
	LANGUAGES CXX
	HOMEPAGE_URL "https://discordcoreapi.com"
	DESCRIPTION "${DESCRIPTION}"
)

set(DiscordCoreAPI-POPCNT_EXITCODE 0 CACHE INTERNAL "For cross-compiling.")
set(DiscordCoreAPI-LZCNT_EXITCODE 0 CACHE INTERNAL "For cross-compiling.")
set(DiscordCoreAPI-BMI_EXITCODE 0 CACHE INTERNAL "For cross-compiling.")
set(DiscordCoreAPI-BMI2_EXITCODE 0 CACHE INTERNAL "For cross-compiling.")
set(DiscordCoreAPI-AVX_EXITCODE 0 CACHE INTERNAL "For cross-compiling.")
set(DiscordCoreAPI-AVX2_EXITCODE 0 CACHE INTERNAL "For cross-compiling.")
set(DiscordCoreAPI-AVX512_EXITCODE 0 CACHE INTERNAL "For cross-compiling.")

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)

add_subdirectory(Library)

if (DISCORDCOREAPI_TEST)
	add_subdirectory("./Tests")
endif()