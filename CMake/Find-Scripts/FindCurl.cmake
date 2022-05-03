# find_curl(RELEASE_ROOT_DIR DEBUG_ROOT_DIR INCLUDE_DIR)
#
# This function locates the Libz library, using a couple of provided paths for searching.
#
# Usage:
#	find_curl(RELEASE_ROOT_DIR DEBUG_ROOT_DIR INCLUDE_DIR)
# Where:
#	RELEASE_ROOT_DIR = The directory containing the RELEASE version of the library, or library's linker file.
#	DEBUG_ROOT_DIR = The directory containing the DEBUG version of the library, or library's linker file.
#	INCLUDE_DIR = The directory containing the public headers.
# What it produces:
#	CURL::Curl
#
function(find_curl RELEASE_ROOT_DIR DEBUG_ROOT_DIR INCLUDE_DIR)
	find_library(
		CURL_RELEASE_LIBRARY
		NAMES "libcurl${LIBRARY_SUFFIX}" "libcurl_imp${LIBRARY_SUFFIX}" "curl${LIBRARY_SUFFIX}" "curl_imp${LIBRARY_SUFFIX}"
		PATHS "${RELEASE_ROOT_DIR}" NO_DEFAULT_PATH
	)
	find_library(
		CURL_DEBUG_LIBRARY
		NAMES "libcurl-d${LIBRARY_SUFFIX}" "libcurl-d_imp${LIBRARY_SUFFIX}" "curl${LIBRARY_SUFFIX}" 
		PATHS "${DEBUG_ROOT_DIR}" NO_DEFAULT_PATH
	)
	if(EXISTS "${CURL_RELEASE_LIBRARY}" AND EXISTS "${CURL_DEBUG_LIBRARY}" AND EXISTS "${INCLUDE_DIR}")
		message(STATUS "Found Curl libraries!")
	else()
		message(FATAL_ERROR "Couldn't find Curl!")
	endif()
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
		add_library(CURL::Curl SHARED IMPORTED GLOBAL)
		set_target_properties(
			CURL::Curl PROPERTIES 
			IMPORTED_LOCATION_RELEASE "${CURL_RELEASE_DLL}" IMPORTED_LOCATION_DEBUG "${CURL_DEBUG_DLL}"
			IMPORTED_IMPLIB_RELEASE "${CURL_RELEASE_LIBRARY}" IMPORTED_IMPLIB_DEBUG "${CURL_DEBUG_LIBRARY}"
		)
		target_include_directories(CURL::Curl INTERFACE "${INCLUDE_DIR}")
		message(STATUS "Found Curl Dlls!")
	else()
		add_library(CURL::Curl STATIC IMPORTED GLOBAL)
		set_target_properties(
			CURL::Curl PROPERTIES 
			IMPORTED_LOCATION_RELEASE "${CURL_RELEASE_LIBRARY}" IMPORTED_LOCATION_DEBUG "${CURL_DEBUG_LIBRARY}"
		)
		target_include_directories(CURL::Curl INTERFACE "${INCLUDE_DIR}")
		unset(CURL_RELEASE_DLL CACHE)
		unset(CURL_DEBUG_DLL CACHE)
		message(STATUS "Couldn't find Curl Dlls - linking statically!")
	endif()
endfunction()