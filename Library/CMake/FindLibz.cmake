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
#	LIBZ_RELEASE_LIBRARY, LIBZ_DEBUG_LIBRARY and in the case of SHARED LIBRARIES - 
#	LIBZ_RELEASE_DLL, and LIBZ_DEBUG_DLL. Which each points to the respective files of each kind. 
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
			list(APPEND RELEASE_LIBRARIES "${LIBZ_RELEASE_LIBRARY}")
			list(APPEND DEBUG_LIBRARIES  "${LIBZ_DEBUG_LIBRARY}")
			set(RELEASE_LIBRARIES "${RELEASE_LIBRARIES}" PARENT_SCOPE)
			set(DEBUG_LIBRARIES  "${DEBUG_LIBRARIES}" PARENT_SCOPE)
			message(STATUS "Found Libz libraries!")
		else()
			message(STATUS "Couldn't find Libz!")
			return()
		endif()
		if (WIN32)
			cmake_path(GET LIBZ_RELEASE_LIBRARY PARENT_PATH LIBZ_RELEASE_FILE_PATH)
			find_file(
				LIBZ_RELEASE_DLL
				NAMES "libz.dll" "zlib.dll"
				PATHS "${LIBZ_RELEASE_FILE_PATH}/" "${LIBZ_RELEASE_FILE_PATH}/../bin/"
				NO_DEFAULT_PATH
			)
			cmake_path(GET LIBZ_DEBUG_LIBRARY PARENT_PATH LIBZ_DEBUG_FILE_PATH)
			find_file(
				LIBZ_DEBUG_DLL
				NAMES "libz.dll" "zlib.dll"
				PATHS "${LIBZ_DEBUG_FILE_PATH}/" "${LIBZ_DEBUG_FILE_PATH}/../bin/"
				NO_DEFAULT_PATH
			)
			if (LIBZ_RELEASE_DLL AND LIBZ_DEBUG_DLL)
				list(APPEND LIBRARY_NAMES "LIBZ")
				set(LIBRARY_NAMES "${LIBRARY_NAMES}" PARENT_SCOPE)
				list(APPEND RELEASE_DLLS "${LIBZ_RELEASE_DLL}")
				list(APPEND DEBUG_DLLS  "${LIBZ_DEBUG_DLL}")
				set(RELEASE_DLLS "${RELEASE_DLLS}" PARENT_SCOPE)
				set(DEBUG_DLLS  "${DEBUG_DLLS}" PARENT_SCOPE)
				message(STATUS "Found Libz Dlls!")
			else()
				list(APPEND LIBRARY_NAMES "LIBZ")
				set(LIBRARY_NAMES "${LIBRARY_NAMES}" PARENT_SCOPE)
				list(APPEND RELEASE_DLLS "NOTFOUND")
				list(APPEND DEBUG_DLLS  "NOTFOUND")
				set(RELEASE_DLLS "${RELEASE_DLLS}" PARENT_SCOPE)
				set(DEBUG_DLLS  "${DEBUG_DLLS}" PARENT_SCOPE)
				unset(LIBZ_RELEASE_DLL CACHE)
				unset(LIBZ_DEBUG_DLL CACHE)
				message(STATUS "Couldn't find Libz Dlls - linking statically!")
			endif()
		endif()
	endif()
endfunction()