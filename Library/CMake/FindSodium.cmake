# find_sodium(RELEASE_ROOT_DIR DEBUG_ROOT_DIR)
#
# This function locates the Libz library, using a couple of provided paths for searching.
#
# Usage:
#	find_sodium(RELEASE_ROOT_DIR DEBUG_ROOT_DIR)
# Where:
#	RELEASE_ROOT_DIR = The directory containing the RELEASE version of the library, or library's linker file.
#	DEBUG_ROOT_DIR = The directory containing the DEBUG version of the library, or library's linker file.
#	INCLUDE_DIR = The directory containing the public headers.
# What it produces:
#	SODIUM::Sodium
#
function(find_sodium RELEASE_ROOT_DIR DEBUG_ROOT_DIR INCLUDE_DIR)
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
			add_library(SODIUM::Sodium SHARED IMPORTED GLOBAL)
			set_target_properties(
				SODIUM::Sodium PROPERTIES 
				IMPORTED_LOCATION_RELEASE "${SODIUM_RELEASE_DLL}" IMPORTED_LOCATION_DEBUG "${SODIUM_DEBUG_DLL}"
				IMPORTED_IMPLIB_RELEASE "${SODIUM_RELEASE_LIBRARY}" IMPORTED_IMPLIB_DEBUG "${SODIUM_DEBUG_LIBRARY}"
			)
			target_include_directories(SODIUM::Sodium INTERFACE "${INCLUDE_DIR}")
			message(STATUS "Found Sodium Dlls!")
		else()
			add_library(SODIUM::Sodium STATIC IMPORTED GLOBAL)
			set_target_properties(
				SODIUM::Sodium PROPERTIES 
				IMPORTED_LOCATION_RELEASE "${SODIUM_RELEASE_LIBRARY}" IMPORTED_LOCATION_DEBUG "${SODIUM_DEBUG_LIBRARY}"
			)
			target_include_directories(SODIUM::Sodium INTERFACE "${INCLUDE_DIR}")
			unset(SODIUM_RELEASE_DLL CACHE)
			unset(SODIUM_DEBUG_DLL CACHE)
			message(STATUS "Couldn't find Sodium Dlls - linking statically!")
		endif()
	endif()
endfunction()