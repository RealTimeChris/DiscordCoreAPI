#[=======================================================================[.rst:
FindCURL
-------

Finds the CURL library.

Imported Targets
^^^^^^^^^^^^^^^^

This module provides the following imported targets, if found:

``CURL::CURL``
  The CURL library

Result Variables
^^^^^^^^^^^^^^^^

This will define the following variables:

``CURL_FOUND``
  True if the system has the CURL library.
``CURL_INCLUDE_DIRS``
  Include directories needed to use CURL.
``CURL_LIBRARIES``
  Libraries needed to link to CURL.

Cache Variables
^^^^^^^^^^^^^^^

The following cache variables may also be set:

``CURL_INCLUDE_DIR``
  The directory containing ``curl.h``.
``CURL_ROOT_DIR``
  The root path to the CURL libraries.

#]=======================================================================]
include(FindPackageHandleStandardArgs)

find_path(CURL_INCLUDE_DIR
  NAMES "curl/curl.h"
  PATHS ${CURL_INCLUDE_DIR}
)

find_library(CURL_RELEASE_LIBRARY
	NAMES "libcurl${LIBRARY_SUFFIX}" "libcurl_imp${LIBRARY_SUFFIX}" "curl${LIBRARY_SUFFIX}" "curl_imp${LIBRARY_SUFFIX}"
	PATHS "${CURL_ROOT_DIR}/release" "${CURL_ROOT_DIR}" NO_DEFAULT_PATH
)
find_library(CURL_DEBUG_LIBRARY
	NAMES "libcurl-d${LIBRARY_SUFFIX}" "libcurl-d_imp${LIBRARY_SUFFIX}" "curl${LIBRARY_SUFFIX}" 
	PATHS "${CURL_ROOT_DIR}/debug" "${CURL_ROOT_DIR}" NO_DEFAULT_PATH
)

cmake_path(GET CURL_RELEASE_LIBRARY PARENT_PATH CURL_RELEASE_FILE_PATH)
find_file(
	CURL_RELEASE_DLL
	NAMES "libcurl.dll" "libcurl.dll" "curl.dll" "libcurl-x64.dll" 
	PATHS "${CURL_RELEASE_FILE_PATH}/" "${CURL_RELEASE_FILE_PATH}/../bin/"
	NO_DEFAULT_PATH
)
cmake_path(GET CURL_DEBUG_LIBRARY PARENT_PATH CURL_DEBUG_FILE_PATH)
find_file(
	CURL_DEBUG_DLL
	NAMES "libcurl-d.dll" "libcurl.dll" "curl-d.dll" "libcurl-x64.dll"
	PATHS "${CURL_DEBUG_FILE_PATH}/" "${CURL_DEBUG_FILE_PATH}/../bin/"
	NO_DEFAULT_PATH
)
if (EXISTS "${CURL_RELEASE_DLL}" AND EXISTS "${CURL_DEBUG_DLL}")
	find_package_handle_standard_args(
		CURL
		FOUND_VAR CURL_FOUND
		REQUIRED_VARS
		CURL_RELEASE_DLL CURL_DEBUG_DLL CURL_RELEASE_LIBRARY CURL_DEBUG_LIBRARY CURL_INCLUDE_DIR 
	)
	if (CURL_FOUND)
		set(CURL_LIBRARIES "${CURL_RELEASE_LIBRARY}" "${CURL_DEBUG_LIBRARY}" "${CURL_RELEASE_DLL}" "${CURL_DEBUG_DLL}")
		set(CURL_INCLUDE_DIRS "${CURL_INCLUDE_DIR}")
		add_library(CURL::CURL SHARED IMPORTED GLOBAL)
		set_target_properties(
			CURL::CURL PROPERTIES 
			IMPORTED_LOCATION_RELEASE "${CURL_RELEASE_DLL}" IMPORTED_LOCATION_DEBUG "${CURL_DEBUG_DLL}"
			IMPORTED_IMPLIB_RELEASE "${CURL_RELEASE_LIBRARY}" IMPORTED_IMPLIB_DEBUG "${CURL_DEBUG_LIBRARY}"
		)
		target_include_directories(CURL::CURL INTERFACE "${INCLUDE_DIR}")
	endif()
else()
	find_package_handle_standard_args(
		CURL
		FOUND_VAR CURL_FOUND
		REQUIRED_VARS
		CURL_RELEASE_LIBRARY CURL_DEBUG_LIBRARY CURL_INCLUDE_DIR 
	)
	if (CURL_FOUND)
		set(CURL_LIBRARIES "${CURL_RELEASE_LIBRARY}" "${CURL_DEBUG_LIBRARY}")
		set(CURL_INCLUDE_DIRS "${CURL_INCLUDE_DIR}")
		add_library(CURL::CURL STATIC IMPORTED GLOBAL)
		set_target_properties(
			CURL::CURL PROPERTIES 
			IMPORTED_LOCATION_RELEASE "${CURL_RELEASE_LIBRARY}" IMPORTED_LOCATION_DEBUG "${CURL_DEBUG_LIBRARY}"
		)
		target_include_directories(CURL::CURL INTERFACE "${INCLUDE_DIR}")
		unset(CURL_RELEASE_DLL CACHE)
		unset(CURL_DEBUG_DLL CACHE)
	endif()
endif()