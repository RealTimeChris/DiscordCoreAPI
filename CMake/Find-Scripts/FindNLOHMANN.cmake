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
# FindNLOHMANN.cmake - This file locates the Nlohmann-Json library, using a provided path for searching.
# May 13, 2021
# https://discordcoreapi.com

# Usage:
#	Set the following directories: NLOHMANN_INCLUDE_DIR
# Where:
#	NLOHMANN_INCLUDE_DIR = The directory containing the public headers.
# What it produces:
#	NLOHMANN::Json
if (UNIX)
	set(LIB_SUFFIX ".a")
	set(LIB_PREFIX "lib")
else()
	set(LIB_SUFFIX ".lib")
	set(LIB_PREFIX "")
endif()
find_file(
	NLOHMANN_JSON_LIBRARY
	NAMES "nlohmann/json.hpp" 
	PATHS "${NLOHMANN_INCLUDE_DIR}"
	NO_DEFAULT_PATH
)
if(EXISTS "${NLOHMANN_JSON_LIBRARY}" AND EXISTS "${NLOHMANN_INCLUDE_DIR}")
	message(STATUS "Found Nlohmann-Json: TRUE")
	add_library(NLOHMANN::Json INTERFACE IMPORTED GLOBAL)
	target_include_directories(NLOHMANN::Json INTERFACE "${NLOHMANN_INCLUDE_DIR}")
else()
	message(FATAL_ERROR "Found Nlohmann-Json: FALSE")
endif()