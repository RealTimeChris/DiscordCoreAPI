# find_glib(RELEASE_ROOT_DIR DEBUG_ROOT_DIR SHARED)
#
# This function locates the Libz library, using a couple of provided paths for searching.
#
# Usage:
#	find_glib(RELEASE_ROOT_DIR DEBUG_ROOT_DIR SHARED)
# Where:
#	RELEASE_ROOT_DIR = The directory containing the RELEASE version of the library, or library's linker file.
#	DEBUG_ROOT_DIR = The directory containing the DEBUG version of the library, or library's linker file.
# What it produces:
#	GLIB_LIBRARY_RELEASE, GLIB_LIBRARY_DEBUG and in the case of SHARED LIBRARIES - 
#	GLIB_RUNTIME_RELEASE, and GLIB_RUNTIME_DEBUG. Which each points to the respective files of each kind. 
#
function(find_glib RELEASE_ROOT_DIR DEBUG_ROOT_DIR SHARED)
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
	if(GLIB_LIBRARY_RELEASE AND GLIB_LIBRARY_DEBUG)
		list(APPEND RELEASE_LIBRARIES "${GLIB_LIBRARY_RELEASE}")
		list(APPEND DEBUG_LIBRARIES  "${GLIB_LIBRARY_DEBUG}")
		set(RELEASE_LIBRARIES "${RELEASE_LIBRARIES}" PARENT_SCOPE)
		set(DEBUG_LIBRARIES  "${DEBUG_LIBRARIES}" PARENT_SCOPE)
		message(STATUS "Found Glib libraries!")
	else()
		message(FATAL_ERROR "Couldn't find Glib!")
		return()
	endif()
	if ("${SHARED}")
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
		if (GLIB_RUNTIME_RELEASE AND GLIB_RUNTIME_DEBUG)
			list(APPEND LIBRARY_NAMES "GLIB")
			set(LIBRARY_NAMES "${LIBRARY_NAMES}" PARENT_SCOPE)
			list(APPEND RELEASE_RUNTIMES "${GLIB_RUNTIME_RELEASE}")
			list(APPEND DEBUG_RUNTIMES  "${GLIB_RUNTIME_DEBUG}")
			set(RELEASE_RUNTIMES "${RELEASE_RUNTIMES}" PARENT_SCOPE)
			set(DEBUG_RUNTIMES  "${DEBUG_RUNTIMES}" PARENT_SCOPE)
			message(STATUS "Found Glib Dlls!")
		else()
			list(APPEND LIBRARY_NAMES "GLIB")
			set(LIBRARY_NAMES "${LIBRARY_NAMES}" PARENT_SCOPE)
			list(APPEND RELEASE_RUNTIMES "NOTFOUND")
			list(APPEND DEBUG_RUNTIMES  "NOTFOUND")
			set(RELEASE_RUNTIMES "${RELEASE_RUNTIMES}" PARENT_SCOPE)
			set(DEBUG_RUNTIMES  "${DEBUG_RUNTIMES}" PARENT_SCOPE)
			unset(GLIB_RUNTIME_RELEASE CACHE)
			unset(GLIB_RUNTIME_DEBUG CACHE)
			message(STATUS "Couldn't find Glib Dlls - linking statically!")
		endif()
	endif()
endfunction()