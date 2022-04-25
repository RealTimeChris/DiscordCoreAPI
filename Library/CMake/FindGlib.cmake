# find_glib(RELEASE_ROOT_DIR DEBUG_ROOT_DIR INCLUDE_DIR)
#
# This function locates the Libz library, using a couple of provided paths for searching.
#
# Usage:
#	find_glib(RELEASE_ROOT_DIR DEBUG_ROOT_DIR INCLUDE_DIR)
# Where:
#	RELEASE_ROOT_DIR = The directory containing the RELEASE version of the library, or library's linker file.
#	DEBUG_ROOT_DIR = The directory containing the DEBUG version of the library, or library's linker file.
#	INCLUDE_DIR = The directory containing the public headers.
# What it produces:
#	GLIB::Glib
#
function(find_glib RELEASE_ROOT_DIR DEBUG_ROOT_DIR INCLUDE_DIR)
	find_library(
		GLIB_RELEASE_LIBRARY
		NAMES "${LIBRARY_PREFIX}glib-2.0${LIBRARY_SUFFIX}" 
		PATHS "${RELEASE_ROOT_DIR}" NO_DEFAULT_PATH
	)
	find_library(
		GLIB_DEBUG_LIBRARY 
		NAMES "${LIBRARY_PREFIX}glib-2.0${LIBRARY_SUFFIX}" 
		PATHS "${DEBUG_ROOT_DIR}" NO_DEFAULT_PATH
	)
	if(GLIB_RELEASE_LIBRARY AND GLIB_DEBUG_LIBRARY)
		message(STATUS "Found Glib libraries!")
	else()
		message(FATAL_ERROR "Couldn't find Glib!")
		return()
	endif()
	if (WIN32)
		cmake_path(GET GLIB_RELEASE_LIBRARY PARENT_PATH GLIB_RELEASE_FILE_PATH)
		find_file(
			GLIB_RELEASE_DLL
			NAMES "libglib-2.0-0.dll" "glib-2.0-0.dll" 
			PATHS "${GLIB_RELEASE_FILE_PATH}/" "${GLIB_RELEASE_FILE_PATH}/../bin/"
			NO_DEFAULT_PATH
		)
		cmake_path(GET GLIB_DEBUG_LIBRARY PARENT_PATH GLIB_DEBUG_FILE_PATH)
		find_file(
			GLIB_DEBUG_DLL
			NAMES "libglib-2.0-0.dll" "glib-2.0-0.dll"
			PATHS "${GLIB_DEBUG_FILE_PATH}/" "${GLIB_DEBUG_FILE_PATH}/../bin/"
			NO_DEFAULT_PATH
		)
		if (GLIB_RELEASE_DLL AND GLIB_DEBUG_DLL)
			add_library(GLIB::Glib SHARED IMPORTED GLOBAL)
			set_target_properties(
				GLIB::Glib PROPERTIES 
				IMPORTED_LOCATION_RELEASE "${GLIB_RELEASE_DLL}" IMPORTED_LOCATION_DEBUG "${GLIB_DEBUG_DLL}"
				IMPORTED_IMPLIB_RELEASE "${GLIB_RELEASE_LIBRARY}" IMPORTED_IMPLIB_DEBUG "${GLIB_DEBUG_LIBRARY}"
			)
			target_include_directories(GLIB::Glib INTERFACE "${INCLUDE_DIR}")
			message(STATUS "Found Glib Dlls!")
		else()
			add_library(GLIB::Glib STATIC IMPORTED GLOBAL)
			set_target_properties(
				GLIB::Glib PROPERTIES 
				IMPORTED_LOCATION_RELEASE "${GLIB_RELEASE_LIBRARY}" IMPORTED_LOCATION_DEBUG "${GLIB_DEBUG_LIBRARY}"
			)
			target_include_directories(GLIB::Glib INTERFACE "${INCLUDE_DIR}")
			unset(GLIB_RELEASE_DLL CACHE)
			unset(GLIB_DEBUG_DLL CACHE)
			message(STATUS "Couldn't find Glib Dlls - linking statically!")
		endif()
	endif()
endfunction()