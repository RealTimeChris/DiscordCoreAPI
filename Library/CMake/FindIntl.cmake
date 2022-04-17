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
		endif()
		message(STATUS "Found Intl!")
	endif()
endfunction()