# find_ffmpeg(RELEASE_ROOT_DIR DEBUG_ROOT_DIR)
#
# This function locates the Curl library, using a couple of provided paths for searching.
#
# Usage:
#	find_ffmpeg(RELEASE_ROOT_DIR DEBUG_ROOT_DIR)
# Where:
#	RELEASE_ROOT_DIR = The directory containing the RELEASE version of the library, or library's linker file.
#	DEBUG_ROOT_DIR = The directory containing the DEBUG version of the library, or library's linker file.
# What it produces:
#	AVCODEC_LIBRARY_RELEASE, AVCODEC_LIBRARY_DEBUG and in the case of SHARED LIBRARIES - 
#	AVCODEC_RUNTIME_RELEASE, and AVCODEC_RUNTIME_DEBUG. Which each points to the respective files of each kind.
#	AVFORMAT_LIBRARY_RELEASE, AVFORMAT_LIBRARY_DEBUG and in the case of SHARED LIBRARIES - 
#	AVFORMAT_RUNTIME_RELEASE, and AVFORMAT_RUNTIME_DEBUG. Which each points to the respective files of each kind.
#	AVUTIL_LIBRARY_RELEASE, AVUTIL_LIBRARY_DEBUG and in the case of SHARED LIBRARIES - 
#	AVUTIL_RUNTIME_RELEASE, and AVUTIL_RUNTIME_DEBUG. Which each points to the respective files of each kind.
#	SWRESAMPLE_LIBRARY_RELEASE, SWRESAMPLE_LIBRARY_DEBUG and in the case of SHARED LIBRARIES - 
#	SWRESAMPLE_RUNTIME_RELEASE, and SWRESAMPLE_RUNTIME_DEBUG. Which each points to the respective files of each kind.
#
function(find_ffmpeg RELEASE_ROOT_DIR DEBUG_ROOT_DIR SHARED)
	find_library(
		AVCODEC_LIBRARY_RELEASE 
		NAMES "${LIBRARY_PREFIX}avcodec${LIBRARY_SUFFIX}" 
		PATHS "${RELEASE_ROOT_DIR}" NO_DEFAULT_PATH
	)
	find_library(
		AVCODEC_LIBRARY_DEBUG 
		NAMES "${LIBRARY_PREFIX}avcodec${LIBRARY_SUFFIX}" 
		PATHS "${DEBUG_ROOT_DIR}" NO_DEFAULT_PATH
	)
	find_library(
		AVFORMAT_LIBRARY_RELEASE 
		NAMES "${LIBRARY_PREFIX}avformat${LIBRARY_SUFFIX}"
		PATHS "${RELEASE_ROOT_DIR}" NO_DEFAULT_PATH
	)
	find_library(
		AVFORMAT_LIBRARY_DEBUG 
		NAMES "${LIBRARY_PREFIX}avformat${LIBRARY_SUFFIX}"
		PATHS "${DEBUG_ROOT_DIR}" NO_DEFAULT_PATH
	)
	find_library(
		AVUTIL_LIBRARY_RELEASE 
		NAMES "${LIBRARY_PREFIX}avutil${LIBRARY_SUFFIX}" 
		PATHS "${RELEASE_ROOT_DIR}" NO_DEFAULT_PATH
	)
	find_library(
		AVUTIL_LIBRARY_DEBUG 
		NAMES "${LIBRARY_PREFIX}avutil${LIBRARY_SUFFIX}"
		PATHS "${DEBUG_ROOT_DIR}" NO_DEFAULT_PATH
	)
	find_library(
		SWRESAMPLE_LIBRARY_RELEASE 
		NAMES "${LIBRARY_PREFIX}swresample${LIBRARY_SUFFIX}"
		PATHS "${RELEASE_ROOT_DIR}" NO_DEFAULT_PATH
	)
	find_library(
		SWRESAMPLE_LIBRARY_DEBUG 
		NAMES "${LIBRARY_PREFIX}swresample${LIBRARY_SUFFIX}"
		PATHS "${DEBUG_ROOT_DIR}" NO_DEFAULT_PATH
	)
	if (AVCODEC_LIBRARY_RELEASE AND AVCODEC_LIBRARY_DEBUG AND AVFORMAT_LIBRARY_RELEASE AND AVFORMAT_LIBRARY_DEBUG AND AVUTIL_LIBRARY_RELEASE
		AND AVUTIL_LIBRARY_DEBUG AND SWRESAMPLE_LIBRARY_RELEASE AND SWRESAMPLE_LIBRARY_DEBUG)
		list(APPEND RELEASE_LIBRARIES_RAW "${AVFORMAT_LIBRARY_RELEASE}")
		list(APPEND DEBUG_LIBRARIES_RAW  "${AVFORMAT_LIBRARY_DEBUG}")
		list(APPEND RELEASE_LIBRARIES_RAW "${AVCODEC_LIBRARY_RELEASE}")
		list(APPEND DEBUG_LIBRARIES_RAW  "${AVCODEC_LIBRARY_DEBUG}")
		list(APPEND RELEASE_LIBRARIES_RAW "${AVUTIL_LIBRARY_RELEASE}")
		list(APPEND DEBUG_LIBRARIES_RAW  "${AVUTIL_LIBRARY_DEBUG}")
		list(APPEND RELEASE_LIBRARIES_RAW "${SWRESAMPLE_LIBRARY_RELEASE}")
		list(APPEND DEBUG_LIBRARIES_RAW  "${SWRESAMPLE_LIBRARY_DEBUG}")
		set(RELEASE_LIBRARIES_RAW "${RELEASE_LIBRARIES_RAW}" PARENT_SCOPE)
		set(DEBUG_LIBRARIES_RAW  "${DEBUG_LIBRARIES_RAW}" PARENT_SCOPE)
		message(STATUS "Found FFMPEG libraries!")
	else()
		message(FATAL_ERROR "Couldn't find FFMPEG!")
		return()
	endif()
	if ("${SHARED}")
		cmake_path(GET AVCODEC_LIBRARY_RELEASE PARENT_PATH AVCODEC_RELEASE_FILE_PATH)
		find_file(
			AVCODEC_RUNTIME_RELEASE
			NAMES "avcodec-58.dll" "libavcodec-58.dll"
			PATHS "${AVCODEC_RELEASE_FILE_PATH}/" "${AVCODEC_RELEASE_FILE_PATH}/../bin/"
			NO_DEFAULT_PATH
		)
		cmake_path(GET AVCODEC_LIBRARY_DEBUG PARENT_PATH AVCODEC_DEBUG_FILE_PATH)
		find_file(
			AVCODEC_RUNTIME_DEBUG
			NAMES "avcodec-58.dll" "libavcodec-58.dll"
			PATHS "${AVCODEC_DEBUG_FILE_PATH}/" "${AVCODEC_DEBUG_FILE_PATH}/../bin/"
			NO_DEFAULT_PATH
		)		
		cmake_path(GET AVFORMAT_LIBRARY_RELEASE PARENT_PATH AVFORMAT_RELEASE_FILE_PATH)
		find_file(
			AVFORMAT_RUNTIME_RELEASE
			NAMES "avformat-58.dll" "libavformat-58.dll"
			PATHS "${AVFORMAT_RELEASE_FILE_PATH}/" "${AVFORMAT_RELEASE_FILE_PATH}/../bin/"
			NO_DEFAULT_PATH
		)
		cmake_path(GET AVFORMAT_LIBRARY_DEBUG PARENT_PATH AVFORMAT_DEBUG_FILE_PATH)
		find_file(
			AVFORMAT_RUNTIME_DEBUG
			NAMES "avformat-58.dll" "avformat-58.dll"
			PATHS "${AVFORMAT_DEBUG_FILE_PATH}/" "${AVFORMAT_DEBUG_FILE_PATH}/../bin/"
			NO_DEFAULT_PATH
		)
		cmake_path(GET AVUTIL_LIBRARY_RELEASE PARENT_PATH AVUTIL_RELEASE_FILE_PATH)
		find_file(
			AVUTIL_RUNTIME_RELEASE
			NAMES "avutil-56.dll" "libavutil-56.dll"
			PATHS "${AVUTIL_RELEASE_FILE_PATH}/" "${AVUTIL_RELEASE_FILE_PATH}/../bin/"
			NO_DEFAULT_PATH
		)
		cmake_path(GET AVUTIL_LIBRARY_DEBUG PARENT_PATH AVUTIL_DEBUG_FILE_PATH)
		find_file(
			AVUTIL_RUNTIME_DEBUG
			NAMES "avutil-56.dll" "libavutil-56.dll"
			PATHS "${AVUTIL_DEBUG_FILE_PATH}/" "${AVUTIL_DEBUG_FILE_PATH}/../bin/"
			NO_DEFAULT_PATH
		)		
		cmake_path(GET SWRESAMPLE_LIBRARY_RELEASE PARENT_PATH SWRESAMPLE_RELEASE_FILE_PATH)
		find_file(
			SWRESAMPLE_RUNTIME_RELEASE
			NAMES "swresample-3.dll" "libswresample-3.dll"
			PATHS "${SWRESAMPLE_RELEASE_FILE_PATH}/" "${SWRESAMPLE_RELEASE_FILE_PATH}/../bin/"
			NO_DEFAULT_PATH
		)
		cmake_path(GET SWRESAMPLE_LIBRARY_DEBUG PARENT_PATH SWRESAMPLE_DEBUG_FILE_PATH)
		find_file(
			SWRESAMPLE_RUNTIME_DEBUG
			NAMES "swresample-3.dll" "libswresample-3.dll"
			PATHS "${SWRESAMPLE_DEBUG_FILE_PATH}/" "${SWRESAMPLE_DEBUG_FILE_PATH}/../bin/"
			NO_DEFAULT_PATH
		)
		if (AVCODEC_RUNTIME_RELEASE AND AVCODEC_RUNTIME_DEBUG AND AVFORMAT_RUNTIME_RELEASE AND AVFORMAT_RUNTIME_DEBUG 
			AND AVUTIL_RUNTIME_RELEASE AND AVUTIL_RUNTIME_DEBUG AND SWRESAMPLE_RUNTIME_RELEASE AND SWRESAMPLE_RUNTIME_DEBUG)
			list(APPEND LIBRARY_NAMES "AVFORMAT")
			list(APPEND RELEASE_RUNTIMES_RAW "${AVFORMAT_RUNTIME_RELEASE}")
			list(APPEND DEBUG_RUNTIMES_RAW  "${AVFORMAT_RUNTIME_DEBUG}")
			list(APPEND LIBRARY_NAMES "AVCODEC")
			list(APPEND RELEASE_RUNTIMES_RAW "${AVCODEC_RUNTIME_RELEASE}")
			list(APPEND DEBUG_RUNTIMES_RAW  "${AVCODEC_RUNTIME_DEBUG}")
			list(APPEND LIBRARY_NAMES "AVUTIL")
			list(APPEND RELEASE_RUNTIMES_RAW "${AVUTIL_RUNTIME_RELEASE}")
			list(APPEND DEBUG_RUNTIMES_RAW  "${AVUTIL_RUNTIME_DEBUG}")
			list(APPEND LIBRARY_NAMES "SWRESAMPLE")
			list(APPEND RELEASE_RUNTIMES_RAW "${SWRESAMPLE_RUNTIME_RELEASE}")
			list(APPEND DEBUG_RUNTIMES_RAW  "${SWRESAMPLE_RUNTIME_DEBUG}")
			set(LIBRARY_NAMES "${LIBRARY_NAMES}" PARENT_SCOPE)
			set(RELEASE_RUNTIMES_RAW "${RELEASE_RUNTIMES_RAW}" PARENT_SCOPE)
			set(DEBUG_RUNTIMES_RAW  "${DEBUG_RUNTIMES_RAW}" PARENT_SCOPE)
			message(STATUS "Found FFMPEG Dlls!")
		else()
			message(STATUS "Couldn't find FFMPEG Dlls!")
			unset(AVCODEC_RUNTIME_RELEASE CACHE)
			unset(AVCODEC_RUNTIME_DEBUG CACHE)
			unset(AVFORMAT_RUNTIME_RELEASE CACHE)
			unset(AVFORMAT_RUNTIME_DEBUG CACHE)
			unset(AVUTIL_RUNTIME_RELEASE CACHE)
			unset(AVUTIL_RUNTIME_DEBUG CACHE)
			unset(SWRESAMPLE_RUNTIME_RELEASE CACHE)
			unset(SWRESAMPLE_RUNTIME_DEBUG CACHE)
		endif()
	endif()
endfunction()