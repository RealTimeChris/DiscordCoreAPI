# find_opus(RELEASE_ROOT_DIR DEBUG_ROOT_DIR)
#
# This function locates the Libz library, using a couple of provided paths for searching.
#
# Usage:
#	find_opus(RELEASE_ROOT_DIR DEBUG_ROOT_DIR)
# Where:
#	RELEASE_ROOT_DIR = The directory containing the RELEASE version of the library, or library's linker file.
#	DEBUG_ROOT_DIR = The directory containing the DEBUG version of the library, or library's linker file.
#	INCLUDE_DIR = The directory containing the public headers.
# What it produces:
#	OPUS::Opus
#
function(find_opus RELEASE_ROOT_DIR DEBUG_ROOT_DIR INCLUDE_DIR)
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
			add_library(OPUS::Opus SHARED IMPORTED GLOBAL)
			set_target_properties(
				OPUS::Opus PROPERTIES 
				IMPORTED_LOCATION_RELEASE "${OPUS_RELEASE_DLL}" IMPORTED_LOCATION_DEBUG "${OPUS_DEBUG_DLL}"
				IMPORTED_IMPLIB_RELEASE "${OPUS_RELEASE_LIBRARY}" IMPORTED_IMPLIB_DEBUG "${OPUS_DEBUG_LIBRARY}"
			)
			target_include_directories(OPUS::Opus INTERFACE "${INCLUDE_DIR}")
			message(STATUS "Found Opus Dlls!")
		else()
			add_library(OPUS::Opus STATIC IMPORTED GLOBAL)
			set_target_properties(
				OPUS::Opus PROPERTIES 
				IMPORTED_LOCATION_RELEASE "${OPUS_RELEASE_LIBRARY}" IMPORTED_LOCATION_DEBUG "${OPUS_DEBUG_LIBRARY}"
			)
			target_include_directories(OPUS::Opus INTERFACE "${INCLUDE_DIR}")
			unset(OPUS_RELEASE_DLL CACHE)
			unset(OPUS_DEBUG_DLL CACHE)
			message(STATUS "Couldn't find Opus Dlls - linking statically!")
		endif()
	endif()
endfunction()