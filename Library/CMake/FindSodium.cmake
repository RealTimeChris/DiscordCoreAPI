# find_sodium(RELEASE_ROOT_DIR DEBUG_ROOT_DIR)
#
# This function locates the Sodium library, using a couple of provided paths for searching.
#
# Usage:
#	find_sodium(RELEASE_ROOT_DIR DEBUG_ROOT_DIR)
# Where:
#	RELEASE_ROOT_DIR = The directory containing the RELEASE version of the library, or library's linker file.
#	DEBUG_ROOT_DIR = The directory containing the DEBUG version of the library, or library's linker file.
# What it produces:
#	SODIUM_LIBRARY_RELEASE, SODIUM_LIBRARY_DEBUG and in the case of SHARED LIBRARIES - 
#	SODIUM_RUNTIME_RELEASE, and SODIUM_RUNTIME_DEBUG. Which each points to the respective files of each kind. 
#
function(find_sodium RELEASE_ROOT_DIR DEBUG_ROOT_DIR)
	find_library(
		SODIUM_LIBRARY_RELEASE 
		NAMES "libsodium${LIBRARY_SUFFIX}" 
		PATHS "${RELEASE_ROOT_DIR}" NO_DEFAULT_PATH
	)
	find_library(
		SODIUM_LIBRARY_DEBUG 
		NAMES "libsodium${LIBRARY_SUFFIX}" 
		PATHS "${DEBUG_ROOT_DIR}" NO_DEFAULT_PATH
	)
	if (SODIUM_LIBRARY_RELEASE AND SODIUM_LIBRARY_DEBUG)
		list(APPEND RELEASE_LIBRARIES_RAW "${SODIUM_LIBRARY_RELEASE}")
		list(APPEND DEBUG_LIBRARIES_RAW  "${SODIUM_LIBRARY_DEBUG}")
		set(RELEASE_LIBRARIES_RAW "${RELEASE_LIBRARIES_RAW}" PARENT_SCOPE)
		set(DEBUG_LIBRARIES_RAW  "${DEBUG_LIBRARIES_RAW}" PARENT_SCOPE)
		if (WIN32)
			cmake_path(GET SODIUM_LIBRARY_RELEASE PARENT_PATH SODIUM_RELEASE_FILE_PATH)
			find_file(
				SODIUM_RUNTIME_RELEASE
				NAMES "libsodium.dll" "sodium.dll" 
				PATHS "${SODIUM_RELEASE_FILE_PATH}/" "${SODIUM_RELEASE_FILE_PATH}/../bin/"
				NO_DEFAULT_PATH
			)
			cmake_path(GET SODIUM_LIBRARY_DEBUG PARENT_PATH SODIUM_DEBUG_FILE_PATH)
			find_file(
				SODIUM_RUNTIME_DEBUG
				NAMES "libsodium.dll" "sodium.dll" 
				PATHS "${SODIUM_DEBUG_FILE_PATH}/" "${SODIUM_DEBUG_FILE_PATH}/../bin/"
				NO_DEFAULT_PATH
			)
			if (NOT SODIUM_RUNTIME_RELEASE OR NOT SODIUM_RUNTIME_DEBUG)
					message(FATAL_ERROR "Couldn't find Sodium Dll!")
			else()
				list(APPEND LIBRARY_NAMES "SODIUM")
				list(APPEND RELEASE_RUNTIMES_RAW "${SODIUM_RUNTIME_RELEASE}")
				list(APPEND DEBUG_RUNTIMES_RAW  "${SODIUM_RUNTIME_DEBUG}")
				set(LIBRARY_NAMES "${LIBRARY_NAMES}" PARENT_SCOPE)
				set(RELEASE_RUNTIMES_RAW "${RELEASE_RUNTIMES_RAW}" PARENT_SCOPE)
				set(DEBUG_RUNTIMES_RAW  "${DEBUG_RUNTIMES_RAW}" PARENT_SCOPE)
			endif()
		endif()
		message(STATUS "Found Sodium!")
	else()
		message(FATAL_ERROR "Couldn't find Sodium!")
	endif()
endfunction()