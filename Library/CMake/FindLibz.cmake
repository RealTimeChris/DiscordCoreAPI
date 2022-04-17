# find_libz(RELEASE_ROOT_DIR DEBUG_ROOT_DIR)
#
# This function locates the Libz library, using a couple of provided paths for searching.
#
# Usage:
#	find_libz(RELEASE_ROOT_DIR DEBUG_ROOT_DIR)
# Where:
#	RELEASE_ROOT_DIR = The directory containing the RELEASE version of the library, or library's linker file.
#	DEBUG_ROOT_DIR = The directory containing the DEBUG version of the library, or library's linker file.
# What it produces:
#	LIBZ_LIBRARY_RELEASE, LIBZ_LIBRARY_DEBUG. Which each points to the respective files of each kind.
#
function(find_libz RELEASE_ROOT_DIR DEBUG_ROOT_DIR SHARED)
	if (UNIX)
		find_library(
			LIBZ_LIBRARY_RELEASE
			NAMES "libz.a" 
			PATHS "${RELEASE_ROOT_DIR}" NO_DEFAULT_PATH
		)
		find_library(
			LIBZ_LIBRARY_DEBUG 
			NAMES "libz.a" 
			PATHS "${DEBUG_ROOT_DIR}" NO_DEFAULT_PATH
		)
		if(LIBZ_LIBRARY_RELEASE AND LIBZ_LIBRARY_DEBUG)
			list(APPEND RELEASE_LIBRARIES_RAW "${LIBZ_LIBRARY_RELEASE}")
			list(APPEND DEBUG_LIBRARIES_RAW  "${LIBZ_LIBRARY_DEBUG}")
			set(RELEASE_LIBRARIES_RAW "${RELEASE_LIBRARIES_RAW}" PARENT_SCOPE)
			set(DEBUG_LIBRARIES_RAW  "${DEBUG_LIBRARIES_RAW}" PARENT_SCOPE)
			message(STATUS "Found Libz libraries!")
		else()
			message(FATAL_ERROR "Couldn't find Libz!")
			return()
		endif()
	endif()
endfunction()