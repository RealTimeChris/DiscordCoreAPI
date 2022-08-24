#
#	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.
#
#	Copyright 2021, 2022 Chris M. (RealTimeChris)
#
#	This file is part of DiscordCoreAPI.
#	DiscordCoreAPI is free software: you can redistribute it and/or modify it under the terms of the GNU
#	General Public License as published by the Free Software Foundation, either version 3 of the License,
#	or (at your option) any later version.
#	DiscordCoreAPI is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
#	even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
#	You should have received a copy of the GNU General Public License along with DiscordCoreAPI.
#	If not, see <https://www.gnu.org/licenses/>.
#
# CMakeLists.txt - The CMake script for building this library.
# May 13, 2021
# https://discordcoreapi.com

cmake_minimum_required(VERSION 3.20)
set(PROJECT_NAME "DiscordCoreAPI")
set(LIB_NAME "${PROJECT_NAME}")
set(PRODUCT_VERSION "1.0.0")
set(DESCRIPTION "A Discord bot library, written in C++, using custom asynchronous coroutines.")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")
set(CMAKE_CONFIGURATION_TYPES "${CMAKE_BUILD_TYPE}")

if (NOT EXISTS "${VCPKG_ROOT}")
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
set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
	set(VCPKG_TARGET_TRIPLET "x64-windows" CACHE STRING "")
	list(APPEND CMAKE_MODULE_PATH "${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/share")
endif()

project(
	"${PROJECT_NAME}"
	VERSION "${PRODUCT_VERSION}"
	LANGUAGES CXX
	HOMEPAGE_URL "https://discordcoreapi.com"
	DESCRIPTION "${DESCRIPTION}"
)

add_subdirectory(Library)