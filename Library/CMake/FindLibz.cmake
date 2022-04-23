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
			NAMES "libz.a" "zlib.lib" "libz.lib"
			PATHS "${RELEASE_ROOT_DIR}" NO_DEFAULT_PATH
		)
		find_library(
			LIBZ_DEBUG_LIBRARY 
			NAMES "libz.a" "zlib.lib" "libz.lib"
			PATHS "${DEBUG_ROOT_DIR}" NO_DEFAULT_PATH
		)
		if(LIBZ_RELEASE_LIBRARY AND LIBZ_DEBUG_LIBRARY)
			set(LIBZ_RELEASE_LIBRARY "${LIBZ_RELEASE_LIBRARY}" PARENT_SCOPE)
			set(LIBZ_DEBUG_LIBRARY "${LIBZ_DEBUG_LIBRARY}" PARENT_SCOPE)
			message(STATUS "Found Libz libraries!")
		else()
			set(LIBZ_RELEASE_LIBRARY "C:/vcpkg/vcpkg.exe" PARENT_SCOPE)
			set(LIBZ_DEBUG_LIBRARY "C:/vcpkg/vcpkg.exe" PARENT_SCOPE)
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
				set(LIBZ_RELEASE_DLL "${LIBZ_RELEASE_DLL}" PARENT_SCOPE)
				set(LIBZ_DEBUG_DLL "${LIBZ_DEBUG_DLL}" PARENT_SCOPE)
				message(STATUS "Found Libz Dlls!")
			else()
				unset(LIBZ_RELEASE_DLL CACHE)
				unset(LIBZ_DEBUG_DLL CACHE)
				message(STATUS "Couldn't find Libz Dlls - linking statically!")
			endif()
		endif()
	endif()
endfunction()