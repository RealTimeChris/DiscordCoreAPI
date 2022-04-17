# find_curl(RELEASE_ROOT_DIR DEBUG_ROOT_DIR)
#
# This function locates the Curl library, using a couple of provided paths for searching.
#
# Usage:
#	find_curl(RELEASE_ROOT_DIR DEBUG_ROOT_DIR)
# Where:
#	RELEASE_ROOT_DIR = The directory containing the RELEASE version of the library, or library's linker file.
#	DEBUG_ROOT_DIR = The directory containing the DEBUG version of the library, or library's linker file.
# What it produces:
#	CURL_LIBRARY_RELEASE, CURL_LIBRARY_DEBUG and in the case of SHARED LIBRARIES - 
#	CURL_RUNTIME_RELEASE, and CURL_RUNTIME_DEBUG. Which each points to the respective files of each kind. 
#
function(find_curl RELEASE_ROOT_DIR DEBUG_ROOT_DIR)
	find_library(
		CURL_LIBRARY_RELEASE
		NAMES "libcurl${LIBRARY_SUFFIX}" "libcurl_imp${LIBRARY_SUFFIX}" "curl${LIBRARY_SUFFIX}" "curl_imp${LIBRARY_SUFFIX}"
		PATHS "${RELEASE_ROOT_DIR}" NO_DEFAULT_PATH
	)
	find_library(
		CURL_LIBRARY_DEBUG
		NAMES "libcurl-d${LIBRARY_SUFFIX}" "libcurl-d_imp${LIBRARY_SUFFIX}" "curl${LIBRARY_SUFFIX}" 
		PATHS "${DEBUG_ROOT_DIR}" NO_DEFAULT_PATH
	)
	if (CURL_LIBRARY_RELEASE AND CURL_LIBRARY_DEBUG)
		list(APPEND RELEASE_LIBRARIES_RAW "${CURL_LIBRARY_RELEASE}")
		list(APPEND DEBUG_LIBRARIES_RAW  "${CURL_LIBRARY_DEBUG}")
		set(RELEASE_LIBRARIES_RAW "${RELEASE_LIBRARIES_RAW}" PARENT_SCOPE)
		set(DEBUG_LIBRARIES_RAW  "${DEBUG_LIBRARIES_RAW}" PARENT_SCOPE)
		if (WIN32)
			cmake_path(GET CURL_LIBRARY_RELEASE PARENT_PATH CURL_RELEASE_FILE_PATH)
			find_file(
				CURL_RUNTIME_RELEASE
				NAMES "libcurl.dll" "libcurl.dll" "curl.dll" "libcurl-x64.dll" 
				PATHS "${CURL_RELEASE_FILE_PATH}/" "${CURL_RELEASE_FILE_PATH}/../bin/"
				NO_DEFAULT_PATH
			)
			cmake_path(GET CURL_LIBRARY_DEBUG PARENT_PATH CURL_DEBUG_FILE_PATH)
			find_file(
				CURL_RUNTIME_DEBUG
				NAMES "libcurl-d.dll" "libcurl.dll" "curl-d.dll" "libcurl-x64.dll"
				PATHS "${CURL_DEBUG_FILE_PATH}/" "${CURL_DEBUG_FILE_PATH}/../bin/"
				NO_DEFAULT_PATH
			)
			if (NOT CURL_RUNTIME_RELEASE OR NOT CURL_RUNTIME_DEBUG)
				message(FATAL_ERROR "Couldn't find Curl Dll!")
			else()
				list(APPEND LIBRARY_NAMES "CURL")
				list(APPEND RELEASE_RUNTIMES_RAW "${CURL_RUNTIME_RELEASE}")
				list(APPEND DEBUG_RUNTIMES_RAW  "${CURL_RUNTIME_DEBUG}")
				set(LIBRARY_NAMES "${LIBRARY_NAMES}" PARENT_SCOPE)
				set(RELEASE_RUNTIMES_RAW "${RELEASE_RUNTIMES_RAW}" PARENT_SCOPE)
				set(DEBUG_RUNTIMES_RAW  "${DEBUG_RUNTIMES_RAW}" PARENT_SCOPE)
			endif()
		endif()
		message(STATUS "Found Curl!")
	else()
		message(FATAL_ERROR "Couldn't find Curl!")
	endif()
endfunction()