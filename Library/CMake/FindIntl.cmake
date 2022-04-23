# find_intl(RELEASE_ROOT_DIR DEBUG_ROOT_DIR)
#
# This function locates the Libz library, using a couple of provided paths for searching.
#
# Usage:
#	find_intl(RELEASE_ROOT_DIR DEBUG_ROOT_DIR)
# Where:
#	RELEASE_ROOT_DIR = The directory containing the RELEASE version of the library, or library's linker file.
#	DEBUG_ROOT_DIR = The directory containing the DEBUG version of the library, or library's linker file.
# What it produces:
#	INTL_RELEASE_LIBRARY, INTL_DEBUG_LIBRARY and in the case of SHARED LIBRARIES - 
#	INTL_RELEASE_DLL, and INTL_DEBUG_DLL. Which each points to the respective files of each kind. 
#
function(find_intl RELEASE_ROOT_DIR DEBUG_ROOT_DIR)
	find_file(
		INTL_RELEASE_LIBRARY
		NAMES "libintl.dll.a" "libintl.lib" "libintl.a"
		PATHS "${RELEASE_ROOT_DIR}" NO_DEFAULT_PATH
	)
	find_file(
		INTL_DEBUG_LIBRARY 
		NAMES "libintl.dll.a" "libintl.lib" "libintl.a"
		PATHS "${DEBUG_ROOT_DIR}" NO_DEFAULT_PATH
	)
	if(INTL_RELEASE_LIBRARY AND INTL_DEBUG_LIBRARY)
		list(APPEND RELEASE_LIBRARIES "${INTL_RELEASE_LIBRARY}")
		list(APPEND DEBUG_LIBRARIES  "${INTL_DEBUG_LIBRARY}")
		set(RELEASE_LIBRARIES "${RELEASE_LIBRARIES}" PARENT_SCOPE)
		set(DEBUG_LIBRARIES  "${DEBUG_LIBRARIES}" PARENT_SCOPE)
		message(STATUS "Found Intl libraries!")
	else()
		message(STATUS "Couldn't find Intl!")
		unset(INTL_RELEASE_LIBRARY CACHE)
		unset(INTL_DEBUG_LIBRARY CACHE)
		return()
	endif()
	if (WIN32)
		cmake_path(GET INTL_RELEASE_LIBRARY PARENT_PATH INTL_RELEASE_FILE_PATH)
		find_file(
			INTL_RELEASE_DLL
			NAMES "libintl-8.dll" "intl-8.dll"
			PATHS "${INTL_RELEASE_FILE_PATH}/" "${INTL_RELEASE_FILE_PATH}/../bin/"
			NO_DEFAULT_PATH
		)
		cmake_path(GET INTL_DEBUG_LIBRARY PARENT_PATH INTL_DEBUG_FILE_PATH)
		find_file(
			INTL_DEBUG_DLL
			NAMES "libintl-8.dll" "intl-8.dll"
			PATHS "${INTL_DEBUG_FILE_PATH}/" "${INTL_DEBUG_FILE_PATH}/../bin/"
			NO_DEFAULT_PATH
		)
		if (INTL_RELEASE_DLL AND INTL_DEBUG_DLL)
			list(APPEND LIBRARY_NAMES "INTL")
			set(LIBRARY_NAMES "${LIBRARY_NAMES}" PARENT_SCOPE)
			list(APPEND RELEASE_DLLS "${INTL_RELEASE_DLL}")
			list(APPEND DEBUG_DLLS  "${INTL_DEBUG_DLL}")
			set(RELEASE_DLLS "${RELEASE_DLLS}" PARENT_SCOPE)
			set(DEBUG_DLLS  "${DEBUG_DLLS}" PARENT_SCOPE)
			message(STATUS "Found Intl Dlls!")
		else()
			list(APPEND LIBRARY_NAMES "INTL")
			set(LIBRARY_NAMES "${LIBRARY_NAMES}" PARENT_SCOPE)
			list(APPEND RELEASE_DLLS "NOTFOUND")
			list(APPEND DEBUG_DLLS  "NOTFOUND")
			set(RELEASE_DLLS "${RELEASE_DLLS}" PARENT_SCOPE)
			set(DEBUG_DLLS  "${DEBUG_DLLS}" PARENT_SCOPE)
			unset(INTL_RELEASE_DLL CACHE)
			unset(INTL_DEBUG_DLL CACHE)
			message(STATUS "Couldn't find Intl Dlls - linking statically!")
		endif()
	endif()
endfunction()