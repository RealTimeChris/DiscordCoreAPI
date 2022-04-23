# find_opus(RELEASE_ROOT_DIR DEBUG_ROOT_DIR)
#
# This function locates the Libz library, using a couple of provided paths for searching.
#
# Usage:
#	find_opus(RELEASE_ROOT_DIR DEBUG_ROOT_DIR)
# Where:
#	RELEASE_ROOT_DIR = The directory containing the RELEASE version of the library, or library's linker file.
#	DEBUG_ROOT_DIR = The directory containing the DEBUG version of the library, or library's linker file.
# What it produces:
#	OPUS_RELEASE_LIBRARY, OPUS_DEBUG_LIBRARY and in the case of SHARED LIBRARIES - 
#	OPUS_RELEASE_DLL, and OPUS_DEBUG_DLL. Which each points to the respective files of each kind. 
#
function(find_opus RELEASE_ROOT_DIR DEBUG_ROOT_DIR)
	find_library(
		OPUS_RELEASE_LIBRARY
		NAMES "${LIBRARY_PREFIX}opus${LIBRARY_SUFFIX}" 
		PATHS "${RELEASE_ROOT_DIR}" NO_DEFAULT_PATH
	)
	find_library(
		OPUS_DEBUG_LIBRARY 
		NAMES "${LIBRARY_PREFIX}opus${LIBRARY_SUFFIX}" 
		PATHS "${DEBUG_ROOT_DIR}" NO_DEFAULT_PATH
	)
	if (OPUS_RELEASE_LIBRARY AND OPUS_DEBUG_LIBRARY)
		list(APPEND RELEASE_LIBRARIES "${OPUS_RELEASE_LIBRARY}")
		list(APPEND DEBUG_LIBRARIES  "${OPUS_DEBUG_LIBRARY}")
		set(RELEASE_LIBRARIES "${RELEASE_LIBRARIES}" PARENT_SCOPE)
		set(DEBUG_LIBRARIES  "${DEBUG_LIBRARIES}" PARENT_SCOPE)
		message(STATUS "Found Opus libraries!")
	else()
		message(FATAL_ERROR "Couldn't find Opus!")
		return()
	endif()
	if (WIN32)
		cmake_path(GET OPUS_RELEASE_LIBRARY PARENT_PATH OPUS_RELEASE_FILE_PATH)
		find_file(
			OPUS_RELEASE_DLL
			NAMES "opus.dll" "libopus.dll"
			PATHS "${OPUS_RELEASE_FILE_PATH}/" "${OPUS_RELEASE_FILE_PATH}/../bin/"
			NO_DEFAULT_PATH
		)
		cmake_path(GET OPUS_DEBUG_LIBRARY PARENT_PATH OPUS_DEBUG_FILE_PATH)
		find_file(
			OPUS_DEBUG_DLL
			NAMES "opus.dll" "libopus.dll"
			PATHS "${OPUS_DEBUG_FILE_PATH}/" "${OPUS_DEBUG_FILE_PATH}/../bin/"
			NO_DEFAULT_PATH
		)
		if (OPUS_RELEASE_DLL AND OPUS_DEBUG_DLL)
			list(APPEND LIBRARY_NAMES "OPUS")
			set(LIBRARY_NAMES "${LIBRARY_NAMES}" PARENT_SCOPE)
			list(APPEND RELEASE_DLLS "${OPUS_RELEASE_DLL}")
			list(APPEND DEBUG_DLLS  "${OPUS_DEBUG_DLL}")
			set(RELEASE_DLLS "${RELEASE_DLLS}" PARENT_SCOPE)
			set(DEBUG_DLLS  "${DEBUG_DLLS}" PARENT_SCOPE)
			message(STATUS "Found Opus Dlls!")
		else()
			list(APPEND LIBRARY_NAMES "OPUS")
			set(LIBRARY_NAMES "${LIBRARY_NAMES}" PARENT_SCOPE)
			list(APPEND RELEASE_DLLS "NOTFOUND")
			list(APPEND DEBUG_DLLS  "NOTFOUND")
			set(RELEASE_DLLS "${RELEASE_DLLS}" PARENT_SCOPE)
			set(DEBUG_DLLS  "${DEBUG_DLLS}" PARENT_SCOPE)
			unset(OPUS_RELEASE_DLL CACHE)
			unset(OPUS_DEBUG_DLL CACHE)
			message(STATUS "Couldn't find Opus Dlls - linking statically!")
		endif()
	endif()
endfunction()