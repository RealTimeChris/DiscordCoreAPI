# find_curl(RELEASE_ROOT_DIR DEBUG_ROOT_DIR)
#
# This function locates the Libz library, using a couple of provided paths for searching.
#
# Usage:
#	find_curl(RELEASE_ROOT_DIR DEBUG_ROOT_DIR)
# Where:
#	RELEASE_ROOT_DIR = The directory containing the RELEASE version of the library, or library's linker file.
#	DEBUG_ROOT_DIR = The directory containing the DEBUG version of the library, or library's linker file.
# What it produces:
#	CURL_RELEASE_LIBRARY, CURL_DEBUG_LIBRARY and in the case of SHARED LIBRARIES - 
#	CURL_RELEASE_DLL, and CURL_DEBUG_DLL. Which each points to the respective files of each kind. 
#
function(find_curl RELEASE_ROOT_DIR DEBUG_ROOT_DIR)
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
	if(CURL_RELEASE_LIBRARY AND CURL_DEBUG_LIBRARY)
		list(APPEND RELEASE_LIBRARIES_RAW "${CURL_RELEASE_LIBRARY}")
		list(APPEND DEBUG_LIBRARIES_RAW  "${CURL_DEBUG_LIBRARY}")
		set(RELEASE_LIBRARIES_RAW "${RELEASE_LIBRARIES_RAW}" PARENT_SCOPE)
		set(DEBUG_LIBRARIES_RAW  "${DEBUG_LIBRARIES_RAW}" PARENT_SCOPE)
		message(STATUS "Found Curl libraries!")
	else()
		message(FATAL_ERROR "Couldn't find Curl!")
		return()
	endif()
	if (WIN32)
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
		if (CURL_RELEASE_DLL AND CURL_DEBUG_DLL)
			list(APPEND LIBRARY_NAMES "CURL")
			set(LIBRARY_NAMES "${LIBRARY_NAMES}" PARENT_SCOPE)
			list(APPEND RELEASE_DLLS_RAW "${CURL_RELEASE_DLL}")
			list(APPEND DEBUG_DLLS_RAW  "${CURL_DEBUG_DLL}")
			set(RELEASE_DLLS_RAW "${RELEASE_DLLS_RAW}" PARENT_SCOPE)
			set(DEBUG_DLLS_RAW  "${DEBUG_DLLS_RAW}" PARENT_SCOPE)
			message(STATUS "Found Curl Dlls!")
		else()
			list(APPEND LIBRARY_NAMES "CURL")
			set(LIBRARY_NAMES "${LIBRARY_NAMES}" PARENT_SCOPE)
			list(APPEND RELEASE_DLLS_RAW "NOTFOUND")
			list(APPEND DEBUG_DLLS_RAW  "NOTFOUND")
			set(RELEASE_DLLS_RAW "${RELEASE_DLLS_RAW}" PARENT_SCOPE)
			set(DEBUG_DLLS_RAW  "${DEBUG_DLLS_RAW}" PARENT_SCOPE)
			unset(CURL_RELEASE_DLL CACHE)
			unset(CURL_DEBUG_DLL CACHE)
			message(STATUS "Couldn't find Curl Dlls - linking statically!")
		endif()
	endif()
endfunction()