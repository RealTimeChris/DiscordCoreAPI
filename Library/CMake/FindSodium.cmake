# find_sodium(RELEASE_ROOT_DIR DEBUG_ROOT_DIR SHARED)
#
# This function locates the Libz library, using a couple of provided paths for searching.
#
# Usage:
#	find_sodium(RELEASE_ROOT_DIR DEBUG_ROOT_DIR SHARED)
# Where:
#	RELEASE_ROOT_DIR = The directory containing the RELEASE version of the library, or library's linker file.
#	DEBUG_ROOT_DIR = The directory containing the DEBUG version of the library, or library's linker file.
# What it produces:
#	SODIUM_LIBRARY_RELEASE, SODIUM_LIBRARY_DEBUG and in the case of SHARED LIBRARIES - 
#	SODIUM_RUNTIME_RELEASE, and SODIUM_RUNTIME_DEBUG. Which each points to the respective files of each kind. 
#
function(find_sodium RELEASE_ROOT_DIR DEBUG_ROOT_DIR SHARED)
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
		list(APPEND RELEASE_LIBRARIES "${SODIUM_LIBRARY_RELEASE}")
		list(APPEND DEBUG_LIBRARIES  "${SODIUM_LIBRARY_DEBUG}")
		set(RELEASE_LIBRARIES "${RELEASE_LIBRARIES}" PARENT_SCOPE)
		set(DEBUG_LIBRARIES  "${DEBUG_LIBRARIES}" PARENT_SCOPE)
		message(STATUS "Found Sodium libraries!")
	else()
		message(FATAL_ERROR "Couldn't find Sodium!")
		return()
	endif()
	if ("${SHARED}")
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
		if (SODIUM_RUNTIME_RELEASE AND SODIUM_RUNTIME_DEBUG)
			list(APPEND LIBRARY_NAMES "SODIUM")
			set(LIBRARY_NAMES "${LIBRARY_NAMES}" PARENT_SCOPE)
			list(APPEND RELEASE_RUNTIMES "${SODIUM_RUNTIME_RELEASE}")
			list(APPEND DEBUG_RUNTIMES  "${SODIUM_RUNTIME_DEBUG}")
			set(RELEASE_RUNTIMES "${RELEASE_RUNTIMES}" PARENT_SCOPE)
			set(DEBUG_RUNTIMES  "${DEBUG_RUNTIMES}" PARENT_SCOPE)
			message(STATUS "Found Sodium Dlls!")
		else()
			list(APPEND LIBRARY_NAMES "SODIUM")
			set(LIBRARY_NAMES "${LIBRARY_NAMES}" PARENT_SCOPE)
			list(APPEND RELEASE_RUNTIMES "NOTFOUND")
			list(APPEND DEBUG_RUNTIMES  "NOTFOUND")
			set(RELEASE_RUNTIMES "${RELEASE_RUNTIMES}" PARENT_SCOPE)
			set(DEBUG_RUNTIMES  "${DEBUG_RUNTIMES}" PARENT_SCOPE)
			unset(SODIUM_RUNTIME_RELEASE CACHE)
			unset(SODIUM_RUNTIME_DEBUG CACHE)
			message(STATUS "Couldn't find Sodium Dlls - linking statically!")
		endif()
	endif()
endfunction()