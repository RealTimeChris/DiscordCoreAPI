# find_glib(RELEASE_ROOT_DIR DEBUG_ROOT_DIR)
#
# This function locates the Glib library, using a couple of provided paths for searching.
#
# Usage:
#	find_glib(RELEASE_ROOT_DIR DEBUG_ROOT_DIR)
# Where:
#	RELEASE_ROOT_DIR = The directory containing the RELEASE version of the library, or library's linker file.
#	DEBUG_ROOT_DIR = The directory containing the DEBUG version of the library, or library's linker file.
# What it produces:
#	GLIB_LIBRARY_RELEASE, GLIB_LIBRARY_DEBUG and in the case of SHARED LIBRARIES - 
#	GLIB_RUNTIME_RELEASE, and GLIB_RUNTIME_DEBUG. Which each points to the respective files of each kind. 
#
function(find_glib RELEASE_ROOT_DIR DEBUG_ROOT_DIR)
	find_library(
		GLIB_LIBRARY_RELEASE
		NAMES "${LIBRARY_PREFIX}glib-2.0${LIBRARY_SUFFIX}" 
		PATHS "${RELEASE_ROOT_DIR}" NO_DEFAULT_PATH
	)
	find_library(
		GLIB_LIBRARY_DEBUG 
		NAMES "${LIBRARY_PREFIX}glib-2.0${LIBRARY_SUFFIX}" 
		PATHS "${DEBUG_ROOT_DIR}" NO_DEFAULT_PATH
	)
	find_file(
		INTL_LIBRARY_FILE
		NAMES "libintl.dll.a" "libintl.lib" "libintl.a"
		PATHS "${RELEASE_ROOT_DIR}" NO_DEFAULT_PATH
	)
	if (GLIB_LIBRARY_RELEASE AND GLIB_LIBRARY_DEBUG)
		list(APPEND RELEASE_LIBRARIES_RAW "${GLIB_LIBRARY_RELEASE}")
		list(APPEND DEBUG_LIBRARIES_RAW  "${GLIB_LIBRARY_DEBUG}")
		set(RELEASE_LIBRARIES_RAW "${RELEASE_LIBRARIES_RAW}" PARENT_SCOPE)
		set(DEBUG_LIBRARIES_RAW  "${DEBUG_LIBRARIES_RAW}" PARENT_SCOPE)
		if (WIN32)
			cmake_path(GET GLIB_LIBRARY_RELEASE PARENT_PATH GLIB_RELEASE_FILE_PATH)
			find_file(
				GLIB_RUNTIME_RELEASE
				NAMES "libglib-2.0-0.dll" "glib-2.0-0.dll" 
				PATHS "${GLIB_RELEASE_FILE_PATH}/" "${GLIB_RELEASE_FILE_PATH}/../bin/"
				NO_DEFAULT_PATH
			)
			cmake_path(GET GLIB_LIBRARY_DEBUG PARENT_PATH GLIB_DEBUG_FILE_PATH)
			find_file(
				GLIB_RUNTIME_DEBUG
				NAMES "libglib-2.0-0.dll" "glib-2.0-0.dll"
				PATHS "${GLIB_DEBUG_FILE_PATH}/" "${GLIB_DEBUG_FILE_PATH}/../bin/"
				NO_DEFAULT_PATH
			)
			set(INTL_FILE_PATH "${RELEASE_ROOT_DIR}")
			cmake_path(GET INTL_LIBRARY_FILE PARENT_PATH INTL_FILE_PATH)
			find_file(
				INTL_RUNTIME_FILE
				NAMES "libintl-8.dll" "intl-8.dll"
				PATHS "${INTL_FILE_PATH}/" "${INTL_FILE_PATH}/../bin/"
				NO_DEFAULT_PATH
			)
			if (NOT GLIB_RUNTIME_RELEASE OR NOT GLIB_RUNTIME_DEBUG)
				message(FATAL_ERROR "Couldn't find Glib Dll!")
			endif()
		endif()
		message(STATUS "Found Glib!")
	else()
		message(FATAL_ERROR "Couldn't find Glib!")
	endif()
endfunction()