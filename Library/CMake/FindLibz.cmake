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
#	LIBZ_LIBRARY_RELEASE, LIBZ_LIBRARY_DEBUG and in the case of SHARED LIBRARIES - 
#	LIBZ_RUNTIME_RELEASE, and LIBZ_RUNTIME_DEBUG. Which each points to the respective files of each kind. 
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
			list(APPEND RELEASE_LIBRARIES "${LIBZ_LIBRARY_RELEASE}")
			list(APPEND DEBUG_LIBRARIES  "${LIBZ_LIBRARY_DEBUG}")
			set(RELEASE_LIBRARIES "${RELEASE_LIBRARIES}" PARENT_SCOPE)
			set(DEBUG_LIBRARIES  "${DEBUG_LIBRARIES}" PARENT_SCOPE)
			message(STATUS "Found Libz libraries!")
		else()
			message(STATUS "Couldn't find Libz!")
			return()
		endif()
		if ("${SHARED}")
			cmake_path(GET LIBZ_LIBRARY_RELEASE PARENT_PATH LIBZ_RELEASE_FILE_PATH)
			find_file(
				LIBZ_RUNTIME_RELEASE
				NAMES "libz.dll" "zlib.dll"
				PATHS "${LIBZ_RELEASE_FILE_PATH}/" "${LIBZ_RELEASE_FILE_PATH}/../bin/"
				NO_DEFAULT_PATH
			)
			cmake_path(GET LIBZ_LIBRARY_DEBUG PARENT_PATH LIBZ_DEBUG_FILE_PATH)
			find_file(
				LIBZ_RUNTIME_DEBUG
				NAMES "libz.dll" "zlib.dll"
				PATHS "${LIBZ_DEBUG_FILE_PATH}/" "${LIBZ_DEBUG_FILE_PATH}/../bin/"
				NO_DEFAULT_PATH
			)
			if (LIBZ_RUNTIME_RELEASE AND LIBZ_RUNTIME_DEBUG)
				list(APPEND LIBRARY_NAMES "LIBZ")
				set(LIBRARY_NAMES "${LIBRARY_NAMES}" PARENT_SCOPE)
				list(APPEND RELEASE_RUNTIMES "${LIBZ_RUNTIME_RELEASE}")
				list(APPEND DEBUG_RUNTIMES  "${LIBZ_RUNTIME_DEBUG}")
				set(RELEASE_RUNTIMES "${RELEASE_RUNTIMES}" PARENT_SCOPE)
				set(DEBUG_RUNTIMES  "${DEBUG_RUNTIMES}" PARENT_SCOPE)
				message(STATUS "Found Libz Dlls!")
			else()
				list(APPEND LIBRARY_NAMES "LIBZ")
				set(LIBRARY_NAMES "${LIBRARY_NAMES}" PARENT_SCOPE)
				list(APPEND RELEASE_RUNTIMES "NOTFOUND")
				list(APPEND DEBUG_RUNTIMES  "NOTFOUND")
				set(RELEASE_RUNTIMES "${RELEASE_RUNTIMES}" PARENT_SCOPE)
				set(DEBUG_RUNTIMES  "${DEBUG_RUNTIMES}" PARENT_SCOPE)
				unset(LIBZ_RUNTIME_RELEASE CACHE)
				unset(LIBZ_RUNTIME_DEBUG CACHE)
				message(STATUS "Couldn't find Libz Dlls - linking statically!")
			endif()
		endif()
	endif()
endfunction()