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
#	LIBZ::Libz
#
function(find_libz RELEASE_ROOT_DIR DEBUG_ROOT_DIR)
	if (UNIX)
		find_library(
			LIBZ_RELEASE_LIBRARY
			NAMES "libz.a" 
			PATHS "${RELEASE_ROOT_DIR}" NO_DEFAULT_PATH
		)
		find_library(
			LIBZ_DEBUG_LIBRARY 
			NAMES "libz.a" 
			PATHS "${DEBUG_ROOT_DIR}" NO_DEFAULT_PATH
		)
		if(LIBZ_RELEASE_LIBRARY AND LIBZ_DEBUG_LIBRARY)
			message(STATUS "Found Libz libraries!")
			add_library(LIBZ::Libz STATIC IMPORTED GLOBAL)
			set_target_properties(
				LIBZ::Libz PROPERTIES 
				IMPORTED_LOCATION_RELEASE "${LIBZ_RELEASE_LIBRARY}" IMPORTED_LOCATION_DEBUG "${LIBZ_DEBUG_LIBRARY}"
			)
		else()
			message(STATUS "Couldn't find Libz!")
			return()
		endif()
	endif()
endfunction()