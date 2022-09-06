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
# FindOPUS.cmake - This file locates the Opus library, using a couple of provided paths for searching.
# May 13, 2021
# https://discordcoreapi.com

# Usage:
#	Set the following directories: OPUS_RELEASE_ROOT, OPUS_DEBUG_ROOT, OPUS_INCLUDE_DIR
# Where:
#	OPUS_RELEASE_ROOT = The directory containing the RELEASE version of the library, or library's linker file.
#	OPUS_DEBUG_ROOT = The directory containing the DEBUG version of the library, or library's linker file.
#	OPUS_INCLUDE_DIR = The directory containing the public headers.
# What it produces:
#	OPUS::Opus
if (UNIX)
	set(LIB_SUFFIX ".a")
	set(LIB_PREFIX "lib")
else()
	set(LIB_SUFFIX ".lib")
	set(LIB_PREFIX "")
endif()
find_library(
	OPUS_DEBUG_LIBRARY 
	NAMES "${LIB_PREFIX}opus${LIB_SUFFIX}" 
	PATHS "${OPUS_DEBUG_ROOT}"
	NO_DEFAULT_PATH
)
find_library(
	OPUS_RELEASE_LIBRARY
	NAMES "${LIB_PREFIX}opus${LIB_SUFFIX}" 
	PATHS "${OPUS_RELEASE_ROOT}"
	NO_DEFAULT_PATH
)
if (EXISTS "${OPUS_DEBUG_LIBRARY}" AND EXISTS "${OPUS_RELEASE_LIBRARY}" AND EXISTS "${OPUS_INCLUDE_DIR}")
	message(STATUS "Found Opus: TRUE")
else()
	message(FATAL_ERROR "Found Opus: FALSE")
endif()
cmake_path(GET OPUS_DEBUG_LIBRARY PARENT_PATH OPUS_DEBUG_FILE_PATH)
find_file(
	OPUS_DEBUG_DLL
	NAMES "opus.dll" "libopus.dll"
	PATHS "${OPUS_DEBUG_FILE_PATH}/" "${OPUS_DEBUG_FILE_PATH}/../bin/"
	NO_DEFAULT_PATH
)
cmake_path(GET OPUS_RELEASE_LIBRARY PARENT_PATH OPUS_RELEASE_FILE_PATH)
find_file(
	OPUS_RELEASE_DLL
	NAMES "opus.dll" "libopus.dll"
	PATHS "${OPUS_RELEASE_FILE_PATH}/" "${OPUS_RELEASE_FILE_PATH}/../bin/"
	NO_DEFAULT_PATH
)
if (EXISTS "${OPUS_DEBUG_DLL}" AND EXISTS "${OPUS_RELEASE_DLL}")
	add_library(OPUS::Opus SHARED IMPORTED GLOBAL)
	set_target_properties(
		OPUS::Opus PROPERTIES 
		IMPORTED_LOCATION_DEBUG "${OPUS_DEBUG_DLL}" IMPORTED_LOCATION_RELEASE "${OPUS_RELEASE_DLL}"
		IMPORTED_IMPLIB_DEBUG "${OPUS_DEBUG_LIBRARY}" IMPORTED_IMPLIB_RELEASE "${OPUS_RELEASE_LIBRARY}"
	)
	target_include_directories(OPUS::Opus INTERFACE "${OPUS_INCLUDE_DIR}")
	message(STATUS "Found Opus Dlls: TRUE")
else()
	add_library(OPUS::Opus STATIC IMPORTED GLOBAL)
	set_target_properties(
		OPUS::Opus PROPERTIES 
		IMPORTED_LOCATION_DEBUG "${OPUS_DEBUG_LIBRARY}" IMPORTED_LOCATION_RELEASE "${OPUS_RELEASE_LIBRARY}"
	)
	target_include_directories(OPUS::Opus INTERFACE "${OPUS_INCLUDE_DIR}")
	unset(OPUS_DEBUG_DLL CACHE)
	unset(OPUS_RELEASE_DLL CACHE)
	message(STATUS "Found Opus Dlls: FALSE - linking statically")
endif()