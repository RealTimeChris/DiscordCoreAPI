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
# FindSIMDJSON.cmake - This file locates the Simdjson library, using a couple of provided paths for searching.
# May 13, 2021
# https://discordcoreapi.com

# Usage:
#	Set the following directories: SIMDJSON_RELEASE_ROOT, SIMDJSON_DEBUG_ROOT, SIMDJSON_INCLUDE_DIR
# Where:
#	SIMDJSON_RELEASE_ROOT = The directory containing the RELEASE version of the library, or library's linker file.
#	SIMDJSON_DEBUG_ROOT = The directory containing the DEBUG version of the library, or library's linker file.
#	SIMDJSON_INCLUDE_DIR = The directory containing the public headers.
# What it produces:
#	SIMDJSON::Simdjson
if (UNIX)
	set(LIB_SUFFIX ".a")
	set(LIB_PREFIX "lib")
else()
	set(LIB_SUFFIX ".lib")
	set(LIB_PREFIX "")
endif()
find_library(
	SIMDJSON_DEBUG_LIBRARY 
	NAMES "${LIB_PREFIX}simdjson${LIB_SUFFIX}" 
	PATHS "${SIMDJSON_DEBUG_ROOT}"
	NO_DEFAULT_PATH
)
find_library(
	SIMDJSON_RELEASE_LIBRARY
	NAMES "${LIB_PREFIX}simdjson${LIB_SUFFIX}" 
	PATHS "${SIMDJSON_RELEASE_ROOT}"
	NO_DEFAULT_PATH
)
if (EXISTS "${SIMDJSON_DEBUG_LIBRARY}" AND EXISTS "${SIMDJSON_RELEASE_LIBRARY}" AND EXISTS "${SIMDJSON_INCLUDE_DIR}")
	message(STATUS "Found Simdjson: TRUE")
else()
	message(FATAL_ERROR "Found Simdjson: FALSE")
endif()
cmake_path(GET SIMDJSON_DEBUG_LIBRARY PARENT_PATH SIMDJSON_DEBUG_FILE_PATH)
find_file(
	SIMDJSON_DEBUG_DLL
	NAMES "simdjson.dll" "libsimdjson.dll"
	PATHS "${SIMDJSON_DEBUG_FILE_PATH}/" "${SIMDJSON_DEBUG_FILE_PATH}/../bin/"
	NO_DEFAULT_PATH
)
cmake_path(GET SIMDJSON_RELEASE_LIBRARY PARENT_PATH SIMDJSON_RELEASE_FILE_PATH)
find_file(
	SIMDJSON_RELEASE_DLL
	NAMES "simdjson.dll" "libsimdjson.dll"
	PATHS "${SIMDJSON_RELEASE_FILE_PATH}/" "${SIMDJSON_RELEASE_FILE_PATH}/../bin/"
	NO_DEFAULT_PATH
)
if (EXISTS "${SIMDJSON_DEBUG_DLL}" AND EXISTS "${SIMDJSON_RELEASE_DLL}")
	add_library(SIMDJSON::Simdjson SHARED IMPORTED GLOBAL)
	set_target_properties(
		SIMDJSON::Simdjson PROPERTIES 
		IMPORTED_LOCATION_DEBUG "${SIMDJSON_DEBUG_DLL}" IMPORTED_LOCATION_RELEASE "${SIMDJSON_RELEASE_DLL}"
		IMPORTED_IMPLIB_DEBUG "${SIMDJSON_DEBUG_LIBRARY}" IMPORTED_IMPLIB_RELEASE "${SIMDJSON_RELEASE_LIBRARY}"
	)
	target_include_directories(SIMDJSON::Simdjson INTERFACE "${SIMDJSON_INCLUDE_DIR}")
	message(STATUS "Found Simdjson Dlls: TRUE")
else()
	add_library(SIMDJSON::Simdjson STATIC IMPORTED GLOBAL)
	set_target_properties(
		SIMDJSON::Simdjson PROPERTIES 
		IMPORTED_LOCATION_DEBUG "${SIMDJSON_DEBUG_LIBRARY}" IMPORTED_LOCATION_RELEASE "${SIMDJSON_RELEASE_LIBRARY}"
	)
	target_include_directories(SIMDJSON::Simdjson INTERFACE "${SIMDJSON_INCLUDE_DIR}")
	unset(SIMDJSON_DEBUG_DLL CACHE)
	unset(SIMDJSON_RELEASE_DLL CACHE)
	message(STATUS "Found Simdjson Dlls: FALSE - linking statically")
endif()