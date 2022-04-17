# find_opus(RELEASE_ROOT_DIR DEBUG_ROOT_DIR)
#
# This function locates the Opus library, using a couple of provided paths for searching.
#
# Usage:
#	find_opus(RELEASE_ROOT_DIR DEBUG_ROOT_DIR)
# Where:
#	RELEASE_ROOT_DIR = The directory containing the RELEASE version of the library, or library's linker file.
#	DEBUG_ROOT_DIR = The directory containing the DEBUG version of the library, or library's linker file.
# What it produces:
#	OPUS_LIBRARY_RELEASE, OPUS_LIBRARY_DEBUG and in the case of SHARED LIBRARIES - 
#	OPUS_RUNTIME_RELEASE, and OPUS_RUNTIME_DEBUG. Which each points to the respective files of each kind. 
#
function(find_opus RELEASE_ROOT_DIR DEBUG_ROOT_DIR)
	find_library(
		OPUS_LIBRARY_RELEASE
		NAMES "${LIBRARY_PREFIX}opus${LIBRARY_SUFFIX}" 
		PATHS "${RELEASE_ROOT_DIR}" NO_DEFAULT_PATH
	)
	find_library(
		OPUS_LIBRARY_DEBUG 
		NAMES "${LIBRARY_PREFIX}opus${LIBRARY_SUFFIX}" 
		PATHS "${DEBUG_ROOT_DIR}" NO_DEFAULT_PATH
	)
	if (OPUS_LIBRARY_RELEASE AND OPUS_LIBRARY_DEBUG)
		list(APPEND RELEASE_LIBRARIES_RAW "${OPUS_LIBRARY_RELEASE}")
		list(APPEND DEBUG_LIBRARIES_RAW  "${OPUS_LIBRARY_DEBUG}")
		set(RELEASE_LIBRARIES_RAW "${RELEASE_LIBRARIES_RAW}" PARENT_SCOPE)
		set(DEBUG_LIBRARIES_RAW  "${DEBUG_LIBRARIES_RAW}" PARENT_SCOPE)
		if (WIN32)
			cmake_path(GET OPUS_LIBRARY_RELEASE PARENT_PATH OPUS_RELEASE_FILE_PATH)
			find_file(
				OPUS_RUNTIME_RELEASE
				NAMES "opus.dll" "libopus.dll"
				PATHS "${OPUS_RELEASE_FILE_PATH}/" "${OPUS_RELEASE_FILE_PATH}/../bin/"
				NO_DEFAULT_PATH
			)
			cmake_path(GET OPUS_LIBRARY_DEBUG PARENT_PATH OPUS_DEBUG_FILE_PATH)
			find_file(
				OPUS_RUNTIME_DEBUG
				NAMES "opus.dll" "libopus.dll"
				PATHS "${OPUS_DEBUG_FILE_PATH}/" "${OPUS_DEBUG_FILE_PATH}/../bin/"
				NO_DEFAULT_PATH
			)
			if (NOT OPUS_RUNTIME_RELEASE OR NOT OPUS_RUNTIME_DEBUG)
				message(WARNING "Couldn't find Opus Dlls!")
				list(APPEND LIBRARY_NAMES "OPUS")
				list(APPEND RELEASE_RUNTIMES_RAW "${OPUS_RUNTIME_RELEASE}")
				list(APPEND DEBUG_RUNTIMES_RAW  "${OPUS_RUNTIME_DEBUG}")
				set(LIBRARY_NAMES "${LIBRARY_NAMES}" PARENT_SCOPE)
				set(RELEASE_RUNTIMES_RAW "${RELEASE_RUNTIMES_RAW}" PARENT_SCOPE)
				set(DEBUG_RUNTIMES_RAW  "${DEBUG_RUNTIMES_RAW}" PARENT_SCOPE)
			else()
			endif()
		endif()
		message(STATUS "Found Opus!")
	else()
		message(FATAL_ERROR "Couldn't find Opus!")
	endif()
endfunction()