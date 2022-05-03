#[=======================================================================[.rst:
FindOPENSSL
-------

Finds the OPENSSL library.

Imported Targets
^^^^^^^^^^^^^^^^

This module provides the following imported targets, if found:

``OPENSSL::Ssl``
``OPENSSL::Crypto``
  The OPENSSL libraries.

Result Variables
^^^^^^^^^^^^^^^^

This will define the following variables:

``OPENSSL_FOUND``
  True if the system has the OPENSSL library.
``OPENSSL_INCLUDE_DIRS``
  Include directories needed to use OPENSSL.
``OPENSSL_LIBRARIES``
  Libraries needed to link to OPENSSL.

Cache Variables
^^^^^^^^^^^^^^^

The following cache variables may also be set:

``OPENSSL_INCLUDE_DIR``
  The directory containing ``curl.h``.
``OPENSSL_ROOT_DIR``
  The root path to the OPENSSL libraries.

#]=======================================================================]
include(FindPackageHandleStandardArgs)

find_path(OPENSSL_INCLUDE_DIR
  NAMES "openssl/ssl.h"
  PATHS ${OPENSSL_INCLUDE_DIR}
)

find_library(
	LIBCRYPTO_RELEASE_LIBRARY 
	NAMES "libcrypto${LIBRARY_SUFFIX}" 
	PATHS "${OPENSSL_ROOT_DIR}/lib" "${OPENSSL_ROOT_DIR}" NO_DEFAULT_PATH
)
find_library(
	LIBCRYPTO_DEBUG_LIBRARY
	NAMES "libcrypto${LIBRARY_SUFFIX}"
	PATHS "${OPENSSL_ROOT_DIR}/debug/lib" "${OPENSSL_ROOT_DIR}" NO_DEFAULT_PATH
)
find_library(
	LIBSSL_RELEASE_LIBRARY 
	NAMES "libssl${LIBRARY_SUFFIX}" 
	PATHS "${OPENSSL_ROOT_DIR}/lib" "${OPENSSL_ROOT_DIR}" NO_DEFAULT_PATH
)
find_library(
	LIBSSL_DEBUG_LIBRARY 
	NAMES "libssl${LIBRARY_SUFFIX}" 
	PATHS "${OPENSSL_ROOT_DIR}/debug/lib" "${OPENSSL_ROOT_DIR}" NO_DEFAULT_PATH
)

cmake_path(GET LIBSSL_RELEASE_LIBRARY PARENT_PATH LIBSSL_RELEASE_FILE_PATH)
find_file(
	LIBSSL_RELEASE_DLL
	NAMES "libssl-3-x64.dll" 
	PATHS "${LIBSSL_RELEASE_FILE_PATH}/" "${LIBSSL_RELEASE_FILE_PATH}/../bin/"
	NO_DEFAULT_PATH
)
cmake_path(GET LIBSSL_DEBUG_LIBRARY PARENT_PATH LIBSSL_DEBUG_FILE_PATH)
find_file(
	LIBSSL_DEBUG_DLL
	NAMES "libssl-3-x64.dll"
	PATHS "${LIBSSL_DEBUG_FILE_PATH}/" "${LIBSSL_DEBUG_FILE_PATH}/../bin/"
	NO_DEFAULT_PATH
)
cmake_path(GET LIBCRYPTO_RELEASE_LIBRARY PARENT_PATH LIBCRYPTO_RELEASE_FILE_PATH)
find_file(
	LIBCRYPTO_RELEASE_DLL
	NAMES "libcrypto-3-x64.dll" 
	PATHS "${LIBCRYPTO_RELEASE_FILE_PATH}/" "${LIBCRYPTO_RELEASE_FILE_PATH}/../bin/"
	NO_DEFAULT_PATH
)
cmake_path(GET LIBCRYPTO_DEBUG_LIBRARY PARENT_PATH LIBCRYPTO_DEBUG_FILE_PATH)
find_file(
	LIBCRYPTO_DEBUG_DLL
	NAMES "libcrypto-3-x64.dll"
	PATHS "${LIBCRYPTO_DEBUG_FILE_PATH}/" "${LIBCRYPTO_DEBUG_FILE_PATH}/../bin/"
	NO_DEFAULT_PATH
)
if (EXISTS "${LIBSSL_RELEASE_DLL}" AND EXISTS "${LIBSSL_DEBUG_DLL}" AND EXISTS "${LIBCRYPTO_RELEASE_DLL}" 
	AND EXISTS "${LIBCRYPTO_DEBUG_DLL}")
	find_package_handle_standard_args(
		OPENSSL
		FOUND_VAR OPENSSL_FOUND
		REQUIRED_VARS
		LIBSSL_RELEASE_DLL LIBSSL_DEBUG_DLL LIBSSL_RELEASE_LIBRARY LIBSSL_DEBUG_LIBRARY 
		LIBCRYPTO_RELEASE_DLL LIBCRYPTO_DEBUG_DLL LIBCRYPTO_RELEASE_LIBRARY LIBCRYPTO_DEBUG_LIBRARY 
		OPENSSL_INCLUDE_DIR 
	)
	set(OPENSSL_LIBRARIES "${LIBSSL_RELEASE_LIBRARY}" "${LIBSSL_DEBUG_LIBRARY}" "${LIBSSL_RELEASE_DLL}" "${LIBSSL_DEBUG_DLL}"
		"${LIBCRYPTO_RELEASE_LIBRARY}" "${LIBCRYPTO_DEBUG_LIBRARY}" "${LIBCRYPTO_RELEASE_DLL}" "${LIBCRYPTO_DEBUG_DLL}")
	set(OPENSSL_INCLUDE_DIRS "${OPENSSL_INCLUDE_DIR}")
	add_library(OPENSSL::Ssl SHARED IMPORTED GLOBAL)
	set_target_properties(
		OPENSSL::Ssl PROPERTIES 
		IMPORTED_LOCATION_RELEASE "${LIBSSL_RELEASE_DLL}" IMPORTED_LOCATION_DEBUG "${LIBSSL_DEBUG_DLL}"
		IMPORTED_IMPLIB_RELEASE "${LIBSSL_RELEASE_LIBRARY}" IMPORTED_IMPLIB_DEBUG "${LIBSSL_DEBUG_LIBRARY}"
	)
	target_include_directories(OPENSSL::Ssl INTERFACE "${INCLUDE_DIR}")
	add_library(OPENSSL::Crypto SHARED IMPORTED GLOBAL)
	set_target_properties(
		OPENSSL::Crypto PROPERTIES 
		IMPORTED_LOCATION_RELEASE "${LIBCRYPTO_RELEASE_DLL}" IMPORTED_LOCATION_DEBUG "${LIBCRYPTO_DEBUG_DLL}"
		IMPORTED_IMPLIB_RELEASE "${LIBCRYPTO_RELEASE_LIBRARY}" IMPORTED_IMPLIB_DEBUG "${LIBCRYPTO_DEBUG_LIBRARY}"
	)
	target_include_directories(OPENSSL::Crypto INTERFACE "${INCLUDE_DIR}")
else()
	find_package_handle_standard_args(
		OPENSSL
		FOUND_VAR OPENSSL_FOUND
		REQUIRED_VARS
		LIBSSL_RELEASE_LIBRARY LIBSSL_DEBUG_LIBRARY 
		LIBCRYPTO_RELEASE_LIBRARY LIBCRYPTO_DEBUG_LIBRARY 
		OPENSSL_INCLUDE_DIR 
	)
	set(OPENSSL_LIBRARIES "${LIBSSL_RELEASE_LIBRARY}" "${LIBSSL_DEBUG_LIBRARY}" "${LIBCRYPTO_RELEASE_LIBRARY}" "${LIBCRYPTO_DEBUG_LIBRARY}")
	set(OPENSSL_INCLUDE_DIRS "${OPENSSL_INCLUDE_DIR}")
	add_library(OPENSSL::Ssl STATIC IMPORTED GLOBAL)
	set_target_properties(
		OPENSSL::Ssl PROPERTIES 
		IMPORTED_LOCATION_RELEASE "${LIBSSL_RELEASE_LIBRARY}" IMPORTED_LOCATION_DEBUG "${LIBSSL_DEBUG_LIBRARY}"
	)
	target_include_directories(OPENSSL::Ssl INTERFACE "${INCLUDE_DIR}")
	add_library(OPENSSL::Crypto STATIC IMPORTED GLOBAL)
	set_target_properties(
		OPENSSL::Crypto PROPERTIES 
		IMPORTED_LOCATION_RELEASE "${LIBCRYPTO_RELEASE_LIBRARY}" IMPORTED_LOCATION_DEBUG "${LIBCRYPTO_DEBUG_LIBRARY}"
	)
	target_include_directories(OPENSSL::Crypto INTERFACE "${INCLUDE_DIR}")
	unset(LIBCRYPTO_RELEASE_DLL CACHE)
	unset(LIBCRYPTO_DEBUG_DLL CACHE)
	unset(LIBSSL_RELEASE_DLL CACHE)
	unset(LIBSSL_DEBUG_DLL CACHE)
endif()