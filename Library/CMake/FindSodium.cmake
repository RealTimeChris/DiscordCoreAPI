# find_sodium(RELEASE_ROOT_DIR DEBUG_ROOT_DIR)
#
# This function locates the Libz library, using a couple of provided paths for searching.
#
# Usage:
#	find_sodium(RELEASE_ROOT_DIR DEBUG_ROOT_DIR)
# Where:
#	RELEASE_ROOT_DIR = The directory containing the RELEASE version of the library, or library's linker file.
#	DEBUG_ROOT_DIR = The directory containing the DEBUG version of the library, or library's linker file.
# What it produces:
#	SODIUM_RELEASE_LIBRARY, SODIUM_DEBUG_LIBRARY and in the case of SHARED LIBRARIES - 
#	SODIUM_RELEASE_DLL, and SODIUM_DEBUG_DLL. Which each points to the respective files of each kind. 
#
function(find_sodium RELEASE_ROOT_DIR DEBUG_ROOT_DIR)
	find_library(
		SODIUM_RELEASE_LIBRARY 
		NAMES "libsodium${LIBRARY_SUFFIX}" 
		PATHS "${RELEASE_ROOT_DIR}" NO_DEFAULT_PATH
	)
	find_library(
		SODIUM_DEBUG_LIBRARY 
		NAMES "libsodium${LIBRARY_SUFFIX}" 
		PATHS "${DEBUG_ROOT_DIR}" NO_DEFAULT_PATH
	)
	if (SODIUM_RELEASE_LIBRARY AND SODIUM_DEBUG_LIBRARY)
		list(APPEND RELEASE_LIBRARIES_RAW "${SODIUM_RELEASE_LIBRARY}")
		list(APPEND DEBUG_LIBRARIES_RAW  "${SODIUM_DEBUG_LIBRARY}")
		set(RELEASE_LIBRARIES_RAW "${RELEASE_LIBRARIES_RAW}" PARENT_SCOPE)
		set(DEBUG_LIBRARIES_RAW  "${DEBUG_LIBRARIES_RAW}" PARENT_SCOPE)
		message(STATUS "Found Sodium libraries!")
	else()
		message(FATAL_ERROR "Couldn't find Sodium!")
		return()
	endif()
	if (WIN32)
		cmake_path(GET SODIUM_RELEASE_LIBRARY PARENT_PATH SODIUM_RELEASE_FILE_PATH)
		find_file(
			SODIUM_RELEASE_DLL
			NAMES "libsodium.dll" "sodium.dll" 
			PATHS "${SODIUM_RELEASE_FILE_PATH}/" "${SODIUM_RELEASE_FILE_PATH}/../bin/"
			NO_DEFAULT_PATH
		)
		cmake_path(GET SODIUM_DEBUG_LIBRARY PARENT_PATH SODIUM_DEBUG_FILE_PATH)
		find_file(
			SODIUM_DEBUG_DLL
			NAMES "libsodium.dll" "sodium.dll" 
			PATHS "${SODIUM_DEBUG_FILE_PATH}/" "${SODIUM_DEBUG_FILE_PATH}/../bin/"
			NO_DEFAULT_PATH
		)
		if (SODIUM_RELEASE_DLL AND SODIUM_DEBUG_DLL)
			list(APPEND LIBRARY_NAMES "SODIUM")
			set(LIBRARY_NAMES "${LIBRARY_NAMES}" PARENT_SCOPE)
			list(APPEND RELEASE_DLLS_RAW "${SODIUM_RELEASE_DLL}")
			list(APPEND DEBUG_DLLS_RAW  "${SODIUM_DEBUG_DLL}")
			set(RELEASE_DLLS_RAW "${RELEASE_DLLS_RAW}" PARENT_SCOPE)
			set(DEBUG_DLLS_RAW  "${DEBUG_DLLS_RAW}" PARENT_SCOPE)
			message(STATUS "Found Sodium Dlls!")
		else()
			list(APPEND LIBRARY_NAMES "SODIUM")
			set(LIBRARY_NAMES "${LIBRARY_NAMES}" PARENT_SCOPE)
			list(APPEND RELEASE_DLLS_RAW "NOTFOUND")
			list(APPEND DEBUG_DLLS_RAW  "NOTFOUND")
			set(RELEASE_DLLS_RAW "${RELEASE_DLLS_RAW}" PARENT_SCOPE)
			set(DEBUG_DLLS_RAW  "${DEBUG_DLLS_RAW}" PARENT_SCOPE)
			unset(SODIUM_RELEASE_DLL CACHE)
			unset(SODIUM_DEBUG_DLL CACHE)
			message(STATUS "Couldn't find Sodium Dlls - linking statically!")
		endif()
	endif()
endfunction()