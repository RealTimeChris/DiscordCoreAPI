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

cmake_minimum_required(VERSION 3.18)

set(PROJECT_NAME "DiscordCoreAPITest")

project(
  "${PROJECT_NAME}"
  VERSION 1.0
)

set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/Install/")

set(CMAKE_CXX_STANDARD 20)

if (GITHUB_BRANCH_TYPE STREQUAL "refs/heads/main")
    set(BRANCH "main")
else()
    set(BRANCH "dev")
	set(DEV true)
endif()

set(BUILD_SHARED_LIBS FALSE)

include(FetchContent)
FetchContent_Declare(
    DiscordCoreAPI
    GIT_REPOSITORY https://github.com/RealTimeChris/DiscordCoreAPI
    GIT_TAG "${BRANCH}"
)
FetchContent_MakeAvailable(DiscordCoreAPI)

add_executable(
    "${PROJECT_NAME}"
    "main.cpp" "./Commands/BotInfo.hpp"
)

target_link_options(
	"${PROJECT_NAME}" INTERFACE	
    "$<$<CXX_COMPILER_ID:CLANG>:-Wl,-rpath,/usr/local/opt/llvm/lib/c++>"
	"$<$<CXX_COMPILER_ID:CLANG>:-L/usr/local/opt/llvm/lib/c++>"
)

target_link_libraries(
	"${PROJECT_NAME}" PRIVATE
	"$<$<CXX_COMPILER_ID:CLANG>:c++abi>"
    "$<$<CXX_COMPILER_ID:CLANG>:c++>"
    DiscordCoreAPI::DiscordCoreAPI
)

if (WIN32)
	install(
		FILES 
		"$<TARGET_PDB_FILE:${PROJECT_NAME}>"
		DESTINATION "${CMAKE_INSTALL_PREFIX}/$<CONFIG>"
	)
	install(
		FILES 
		"$<IF:$<CONFIG:Debug>,${DEBUG_PDB_FILE_PATH},${RELEASE_PDB_FILE_PATH}>" 
		DESTINATION "${CMAKE_INSTALL_PREFIX}/$<CONFIG>"
	)	
	install(
		FILES 
		"$<TARGET_RUNTIME_DLLS:${PROJECT_NAME}>"
		DESTINATION "${CMAKE_INSTALL_PREFIX}/$<CONFIG>"
	)
endif()

install(
	FILES 
	"$<TARGET_FILE:${PROJECT_NAME}>"
	DESTINATION "$<CONFIG>"
)