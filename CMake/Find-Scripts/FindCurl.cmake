# This file locates the Curl library, using a couple of provided paths for searching.
#
# Usage:
#	Set the following directories: CURL_RELEASE_ROOT, CURL_DEBUG_ROOT, CURL_INCLUDE_DIR
# Where:
#	CURL_RELEASE_ROOT = The directory containing the RELEASE version of the library, or library's linker file.
#	CURL_DEBUG_ROOT = The directory containing the DEBUG version of the library, or library's linker file.
#	CURL_INCLUDE_DIR = The directory containing the public headers.
# What it produces:
#	CURL::Curl
if (UNIX)
	set(LIB_SUFFIX ".a")
	set(LIB_PREFIX "lib")
else()
	set(LIB_SUFFIX ".lib")
	set(LIB_PREFIX "")
endif()
find_library(
	CURL_RELEASE_LIBRARY
	NAMES "libcurl${LIB_SUFFIX}" "libcurl_imp${LIB_SUFFIX}" "curl${LIB_SUFFIX}" "curl_imp${LIB_SUFFIX}"
	PATHS "${CURL_RELEASE_ROOT}" NO_DEFAULT_PATH
)
find_library(
	CURL_DEBUG_LIBRARY
	NAMES "libcurl-d${LIB_SUFFIX}" "libcurl-d_imp${LIB_SUFFIX}" "curl${LIB_SUFFIX}" 
	PATHS "${CURL_DEBUG_ROOT}" NO_DEFAULT_PATH
)
if(EXISTS "${CURL_RELEASE_LIBRARY}" AND EXISTS "${CURL_DEBUG_LIBRARY}" AND EXISTS "${CURL_INCLUDE_DIR}")
	message(STATUS "Found Curl libraries")
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
	target_include_directories(CURL::Curl INTERFACE "${CURL_INCLUDE_DIR}")
	message(STATUS "Found Curl Dlls")
else()
	add_library(CURL::Curl STATIC IMPORTED GLOBAL)
	set_target_properties(
		CURL::Curl PROPERTIES 
		IMPORTED_LOCATION_RELEASE "${CURL_RELEASE_LIBRARY}" IMPORTED_LOCATION_DEBUG "${CURL_DEBUG_LIBRARY}"
	)
	target_include_directories(CURL::Curl INTERFACE "${CURL_INCLUDE_DIR}")
	unset(CURL_RELEASE_DLL CACHE)
	unset(CURL_DEBUG_DLL CACHE)
	message(STATUS "Couldn't find Curl Dlls - linking statically")
endif()
# This file locates the Libz library, using a couple of provided paths for searching.
#
# Usage:
#	Set the following directories: CURL_RELEASE_ROOT, CURL_DEBUG_ROOT, CURL_INCLUDE_DIR
# Where:
#	CURL_RELEASE_ROOT = The directory containing the RELEASE version of the library, or library's linker file.
#	CURL_DEBUG_ROOT = The directory containing the DEBUG version of the library, or library's linker file.
#	CURL_INCLUDE_DIR = The directory containing the public headers.
# What it produces:
#	LIBZ::Libz
if (UNIX)
	find_library(
		LIBZ_RELEASE_LIBRARY
		NAMES "libz.a" 
		PATHS "${CURL_RELEASE_ROOT}" NO_DEFAULT_PATH
	)
	find_library(
		LIBZ_DEBUG_LIBRARY 
		NAMES "libz.a" 
		PATHS "${CURL_DEBUG_ROOT}" NO_DEFAULT_PATH
	)
	if(EXISTS "${LIBZ_RELEASE_LIBRARY}" AND EXISTS "${LIBZ_DEBUG_LIBRARY}" AND EXISTS "${CURL_INCLUDE_DIR}")
		message(STATUS "Found Libz libraries")
		add_library(LIBZ::Libz STATIC IMPORTED GLOBAL)
		set_target_properties(
			LIBZ::Libz PROPERTIES 
			IMPORTED_LOCATION_RELEASE "${LIBZ_RELEASE_LIBRARY}" IMPORTED_LOCATION_DEBUG "${LIBZ_DEBUG_LIBRARY}"
		)
		target_include_directories(LIBZ::Libz INTERFACE "${CURL_INCLUDE_DIR}")
	else()
		message(STATUS "Couldn't find Libz!")
	endif()
endif()