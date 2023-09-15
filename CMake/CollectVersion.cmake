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
# CollectVersion.cmake - The script for collecting the library's version..
# May 13, 2021
# https://discordcoreapi.com

function(collect_version project_name)

	set(PRODUCT_VERSION "")
	file(READ "${CMAKE_CURRENT_SOURCE_DIR}/Vcpkg/ports/${project_name}/vcpkg.json" JSON_STRING)
	foreach(IDX RANGE 1)
		string(JSON PRODUCT_VERSION GET "${JSON_STRING}" "version")
	endforeach()
	string(REPLACE "." ";" PRODUCT_VERSION_LIST "${PRODUCT_VERSION}")
	list(GET PRODUCT_VERSION_LIST 0 VERSION_MAJOR)
	list(APPEND PRODUCT_VERSION_NEW "${VERSION_MAJOR}")
	set(VERSION_MAJOR "${VERSION_MAJOR}" PARENT_SCOPE)
	list(GET PRODUCT_VERSION_LIST 1 VERSION_MINOR)
	list(APPEND PRODUCT_VERSION_NEW  "${VERSION_MINOR}")
	set(VERSION_MINOR "${VERSION_MINOR}" PARENT_SCOPE)
	list(LENGTH PRODUCT_VERSION_LIST LIST_SIZE)
	if (LIST_SIZE GREATER 2)
		list(GET PRODUCT_VERSION_LIST 2 VERSION_PATCH)
		set(VERSION_PATCH "${VERSION_PATCH}" PARENT_SCOPE)
		list(APPEND PRODUCT_VERSION_NEW  "${VERSION_PATCH}")
	endif()
	string(REPLACE ";" "." PRODUCT_VERSION_NEW  "${PRODUCT_VERSION_NEW}")
	set(PRODUCT_VERSION "${PRODUCT_VERSION_NEW}" PARENT_SCOPE)

endfunction()