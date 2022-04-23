# find_glib(RELEASE_ROOT_DIR DEBUG_ROOT_DIR)
#
# This function locates the Libz library, using a couple of provided paths for searching.
#
# Usage:
#	find_glib(RELEASE_ROOT_DIR DEBUG_ROOT_DIR)
# Where:
#	RELEASE_ROOT_DIR = The directory containing the RELEASE version of the library, or library's linker file.
#	DEBUG_ROOT_DIR = The directory containing the DEBUG version of the library, or library's linker file.
# What it produces:
#	GLIB_RELEASE_LIBRARY, GLIB_DEBUG_LIBRARY and in the case of SHARED LIBRARIES - 
#	GLIB_RELEASE_DLL, and GLIB_DEBUG_DLL. Which each points to the respective files of each kind. 
#
function(find_glib RELEASE_ROOT_DIR DEBUG_ROOT_DIR)
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
		list(APPEND RELEASE_LIBRARIES_RAW "${GLIB_RELEASE_LIBRARY}")
		list(APPEND DEBUG_LIBRARIES_RAW  "${GLIB_DEBUG_LIBRARY}")
		set(RELEASE_LIBRARIES_RAW "${RELEASE_LIBRARIES_RAW}" PARENT_SCOPE)
		set(DEBUG_LIBRARIES_RAW  "${DEBUG_LIBRARIES_RAW}" PARENT_SCOPE)
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
			list(APPEND LIBRARY_NAMES "GLIB")
			set(LIBRARY_NAMES "${LIBRARY_NAMES}" PARENT_SCOPE)
			list(APPEND RELEASE_DLLS "${GLIB_RELEASE_DLL}")
			list(APPEND DEBUG_DLLS  "${GLIB_DEBUG_DLL}")
			set(RELEASE_DLLS "${RELEASE_DLLS}" PARENT_SCOPE)
			set(DEBUG_DLLS  "${DEBUG_DLLS}" PARENT_SCOPE)
			message(STATUS "Found Glib Dlls!")
		else()
			list(APPEND LIBRARY_NAMES "GLIB")
			set(LIBRARY_NAMES "${LIBRARY_NAMES}" PARENT_SCOPE)
			list(APPEND RELEASE_DLLS "NOTFOUND")
			list(APPEND DEBUG_DLLS  "NOTFOUND")
			set(RELEASE_DLLS "${RELEASE_DLLS}" PARENT_SCOPE)
			set(DEBUG_DLLS  "${DEBUG_DLLS}" PARENT_SCOPE)
			unset(GLIB_RELEASE_DLL CACHE)
			unset(GLIB_DEBUG_DLL CACHE)
			message(STATUS "Couldn't find Glib Dlls - linking statically!")
		endif()
	endif()
endfunction()