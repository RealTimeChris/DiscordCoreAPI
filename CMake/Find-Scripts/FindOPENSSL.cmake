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
# FindOPENSSL.cmake - This file locates the OpenSSL libraries, using a couple of provided paths for searching.
# May 13, 2021
# https://discordcoreapi.com

# Usage:
# Set the following directories: OPENSSL_RELEASE_ROOT, OPENSSL_DEBUG_ROOT, OPENSSL_INCLUDE_DIR
# Where:
#	OPENSSL_RELEASE_ROOT = The directory containing the RELEASE version of the library, or library's linker file.
#	OPENSSL_DEBUG_ROOT = The directory containing the DEBUG version of the library, or library's linker file.
#	OPENSSL_INCLUDE_DIR = The directory containing the public headers.
# What it produces:
#	OPENSSL::Crypto and OPENSLL::Ssl
if (UNIX)
	set(LIB_SUFFIX ".a")
	set(LIB_PREFIX "lib")
else()
	set(LIB_SUFFIX ".lib")
	set(LIB_PREFIX "")
endif()
find_library(
	LIBCRYPTO_DEBUG_LIBRARY
	NAMES "libcrypto${LIB_SUFFIX}"
	PATHS "${OPENSSL_DEBUG_ROOT}"
	NO_DEFAULT_PATH
)
find_library(
	LIBCRYPTO_RELEASE_LIBRARY 
	NAMES "libcrypto${LIB_SUFFIX}" 
	PATHS "${OPENSSL_RELEASE_ROOT}"
	NO_DEFAULT_PATH
)
find_library(
	LIBSSL_DEBUG_LIBRARY 
	NAMES "libssl${LIB_SUFFIX}" 
	PATHS "${OPENSSL_DEBUG_ROOT}"
	NO_DEFAULT_PATH
)
find_library(
	LIBSSL_RELEASE_LIBRARY 
	NAMES "libssl${LIB_SUFFIX}" 
	PATHS "${OPENSSL_RELEASE_ROOT}"
	NO_DEFAULT_PATH
)
if (EXISTS "${LIBSSL_DEBUG_LIBRARY}" AND EXISTS "${LIBSSL_RELEASE_LIBRARY}" AND EXISTS "${LIBCRYPTO_DEBUG_LIBRARY}" 
	AND EXISTS "${LIBCRYPTO_RELEASE_LIBRARY}" AND EXISTS "${OPENSSL_INCLUDE_DIR}")
	message(STATUS "Found OpenSSL: TRUE")
else()
	message(FATAL_ERROR "Found OpenSSL: FALSE")
endif()
cmake_path(GET LIBSSL_DEBUG_LIBRARY PARENT_PATH LIBSSL_DEBUG_FILE_PATH)
find_file(
	LIBSSL_DEBUG_DLL
	NAMES "libssl-3-x64.dll"
	PATHS "${LIBSSL_DEBUG_FILE_PATH}/" "${LIBSSL_DEBUG_FILE_PATH}/../bin/"
	NO_DEFAULT_PATH
)
cmake_path(GET LIBSSL_RELEASE_LIBRARY PARENT_PATH LIBSSL_RELEASE_FILE_PATH)
find_file(
	LIBSSL_RELEASE_DLL
	NAMES "libssl-3-x64.dll" 
	PATHS "${LIBSSL_RELEASE_FILE_PATH}/" "${LIBSSL_RELEASE_FILE_PATH}/../bin/"
	NO_DEFAULT_PATH
)
cmake_path(GET LIBCRYPTO_DEBUG_LIBRARY PARENT_PATH LIBCRYPTO_DEBUG_FILE_PATH)
find_file(
	LIBCRYPTO_DEBUG_DLL
	NAMES "libcrypto-3-x64.dll"
	PATHS "${LIBCRYPTO_DEBUG_FILE_PATH}/" "${LIBCRYPTO_DEBUG_FILE_PATH}/../bin/"
	NO_DEFAULT_PATH
)
cmake_path(GET LIBCRYPTO_RELEASE_LIBRARY PARENT_PATH LIBCRYPTO_RELEASE_FILE_PATH)
find_file(
	LIBCRYPTO_RELEASE_DLL
	NAMES "libcrypto-3-x64.dll" 
	PATHS "${LIBCRYPTO_RELEASE_FILE_PATH}/" "${LIBCRYPTO_RELEASE_FILE_PATH}/../bin/"
	NO_DEFAULT_PATH
)
if (EXISTS "${LIBSSL_DEBUG_DLL}" AND EXISTS "${LIBSSL_RELEASE_DLL}" AND EXISTS "${LIBCRYPTO_DEBUG_DLL}" 
	AND EXISTS "${LIBCRYPTO_RELEASE_DLL}")
	add_library(OPENSSL::Ssl SHARED IMPORTED GLOBAL)
	set_target_properties(
		OPENSSL::Ssl PROPERTIES 
		IMPORTED_LOCATION_DEBUG "${LIBSSL_DEBUG_DLL}" IMPORTED_LOCATION_RELEASE "${LIBSSL_RELEASE_DLL}"
		IMPORTED_IMPLIB_DEBUG "${LIBSSL_DEBUG_LIBRARY}" IMPORTED_IMPLIB_RELEASE "${LIBSSL_RELEASE_LIBRARY}"
	)
	target_include_directories(OPENSSL::Ssl INTERFACE "${OPENSSL_INCLUDE_DIR}")
	add_library(OPENSSL::Crypto SHARED IMPORTED GLOBAL)
	set_target_properties(
		OPENSSL::Crypto PROPERTIES 
		IMPORTED_LOCATION_DEBUG "${LIBCRYPTO_DEBUG_DLL}" IMPORTED_LOCATION_RELEASE "${LIBCRYPTO_RELEASE_DLL}"
		IMPORTED_IMPLIB_DEBUG "${LIBCRYPTO_DEBUG_LIBRARY}" IMPORTED_IMPLIB_RELEASE "${LIBCRYPTO_RELEASE_LIBRARY}"
	)
	target_include_directories(OPENSSL::Crypto INTERFACE "${OPENSSL_INCLUDE_DIR}")
	message(STATUS "Found OpenSSL Dlls: TRUE")
else()
	add_library(OPENSSL::Ssl STATIC IMPORTED GLOBAL)
	set_target_properties(
		OPENSSL::Ssl PROPERTIES 
		IMPORTED_LOCATION_DEBUG "${LIBSSL_DEBUG_LIBRARY}" IMPORTED_LOCATION_RELEASE "${LIBSSL_RELEASE_LIBRARY}"
	)
	target_include_directories(OPENSSL::Ssl INTERFACE "${OPENSSL_INCLUDE_DIR}")
	add_library(OPENSSL::Crypto STATIC IMPORTED GLOBAL)
	set_target_properties(
		OPENSSL::Crypto PROPERTIES 
		IMPORTED_LOCATION_DEBUG "${LIBCRYPTO_DEBUG_LIBRARY}" IMPORTED_LOCATION_RELEASE "${LIBCRYPTO_RELEASE_LIBRARY}"
	)
	target_include_directories(OPENSSL::Crypto INTERFACE "${OPENSSL_INCLUDE_DIR}")
	unset(LIBCRYPTO_DEBUG_DLL CACHE)
	unset(LIBCRYPTO_RELEASE_DLL CACHE)
	unset(LIBSSL_DEBUG_DLL CACHE)
	unset(LIBSSL_RELEASE_DLL CACHE)
	message(STATUS "Found OpenSSL Dlls: FALSE - linking statically")
endif()
