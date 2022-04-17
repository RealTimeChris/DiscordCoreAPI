# find_intl(ROOT_DIR)
#
# This function locates the Opus library, using a couple of provided paths for searching.
#
# Usage:
#	find_intl(ROOT_DIR DEBUG_ROOT_DIR)
# Where:
#	ROOT_DIR = The directory containing the library, or library's linker file.
# What it produces:
#	INTL_LIBRARY and in the case of SHARED LIBRARIES - 
#	INTL_RUNTIME_FILE. Which each points to the respective files of each kind.
#
function(find_intl ROOT_DIR)
	if (WIN32)
		find_file(
			INTL_LIBRARY
			NAMES "libintl.dll.a" "libintl.lib" "libintl.a"
			PATHS "${ROOT_DIR}" NO_DEFAULT_PATH
		)
		list(APPEND RELEASE_LIBRARIES_RAW "${INTL_LIBRARY}")
		list(APPEND DEBUG_LIBRARIES_RAW  "${INTL_LIBRARY}")
		set(RELEASE_LIBRARIES_RAW "${RELEASE_LIBRARIES_RAW}" PARENT_SCOPE)
		set(DEBUG_LIBRARIES_RAW  "${DEBUG_LIBRARIES_RAW}" PARENT_SCOPE)
		set(INTL_FILE_PATH "${ROOT_DIR}")
		cmake_path(GET INTL_LIBRARY PARENT_PATH INTL_FILE_PATH)
		find_file(
			INTL_RUNTIME_FILE
			NAMES "libintl-8.dll" "intl-8.dll"
			PATHS "${INTL_FILE_PATH}/" "${INTL_FILE_PATH}/../bin/"
			NO_DEFAULT_PATH
		)
		if (NOT INTL_RUNTIME_FILE)
			message(FATAL_ERROR "Couldn't find Intl Dll!")
		else()
			list(APPEND LIBRARY_NAMES "INTL")				
			list(APPEND RELEASE_RUNTIMES_RAW "${INTL_RUNTIME_FILE}")
			list(APPEND DEBUG_RUNTIMES_RAW  "${INTL_RUNTIME_FILE}")
			set(LIBRARY_NAMES "${LIBRARY_NAMES}" PARENT_SCOPE)
			set(RELEASE_RUNTIMES_RAW "${RELEASE_RUNTIMES_RAW}" PARENT_SCOPE)
			set(DEBUG_RUNTIMES_RAW  "${DEBUG_RUNTIMES_RAW}" PARENT_SCOPE)
		endif()
		message(STATUS "Found Intl!")
	endif()
endfunction()