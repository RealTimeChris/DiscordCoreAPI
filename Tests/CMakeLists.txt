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
set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/Install/")

add_executable(
    "DiscordCoreAPITest"
    "main.cpp" "./Commands/BotInfo.hpp"
)

target_link_libraries(
	"DiscordCoreAPITest" PRIVATE
    DiscordCoreAPI::DiscordCoreAPI
	Jsonifier::Jsonifier
)

target_compile_options(
	"DiscordCoreAPITest" PUBLIC
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
	"DiscordCoreAPITest" PUBLIC
	"$<$<CXX_COMPILER_ID:GNU>:$<$<STREQUAL:${ASAN_ENABLED},TRUE>:-fsanitize=address>>"
	"$<$<CXX_COMPILER_ID:CLANG>:$<$<STREQUAL:${ASAN_ENABLED},TRUE>:-fsanitize=address>>"
	"$<$<CXX_COMPILER_ID:MSVC>:/DEBUG>"
)

if (WIN32)
	install(
		FILES 
		"$<TARGET_PDB_FILE:DiscordCoreAPITest>"
		DESTINATION "${CMAKE_INSTALL_PREFIX}/$<CONFIG>"
	)
	install(
		FILES 
		"$<IF:$<CONFIG:Debug>,${DEBUG_PDB_FILE_PATH},${RELEASE_PDB_FILE_PATH}>" 
		DESTINATION "${CMAKE_INSTALL_PREFIX}/$<CONFIG>"
	)	
	install(
		FILES 
		"$<TARGET_RUNTIME_DLLS:DiscordCoreAPITest>"
		DESTINATION "${CMAKE_INSTALL_PREFIX}/$<CONFIG>"
	)
endif()

install(
	FILES 
	"$<TARGET_FILE:DiscordCoreAPITest>"
	DESTINATION "$<CONFIG>"
)