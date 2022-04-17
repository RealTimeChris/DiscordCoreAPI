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
function(find_intl ROOT_DIR SHARED)
	find_file(
		INTL_LIBRARY
		NAMES "libintl.dll.a" "libintl.lib" "libintl.a"
		PATHS "${ROOT_DIR}" NO_DEFAULT_PATH
	)
	if(INTL_LIBRARY)
		list(APPEND RELEASE_LIBRARIES "${INTL_LIBRARY}")
		list(APPEND DEBUG_LIBRARIES  "${INTL_LIBRARY}")
		set(RELEASE_LIBRARIES "${RELEASE_LIBRARIES}" PARENT_SCOPE)
		set(DEBUG_LIBRARIES  "${DEBUG_LIBRARIES}" PARENT_SCOPE)
		message(STATUS "Found Intl library!")
	else()
		message(STATUS "Couldn't find Intl!")
		unset(INTL_LIBRARY)
		return()
	endif()
	if ("${SHARED}")
		set(INTL_FILE_PATH "${ROOT_DIR}")
		cmake_path(GET INTL_LIBRARY PARENT_PATH INTL_FILE_PATH)
		find_file(
			INTL_RUNTIME_FILE
			NAMES "libintl-8.dll" "intl-8.dll"
			PATHS "${INTL_FILE_PATH}/" "${INTL_FILE_PATH}/../bin/"
			NO_DEFAULT_PATH
		)
		if (INTL_RUNTIME_FILE)
			list(APPEND LIBRARY_NAMES "INTL")				
			list(APPEND RELEASE_RUNTIMES "${INTL_RUNTIME_FILE}")
			list(APPEND DEBUG_RUNTIMES  "${INTL_RUNTIME_FILE}")
			set(LIBRARY_NAMES "${LIBRARY_NAMES}" PARENT_SCOPE)
			set(RELEASE_RUNTIMES "${RELEASE_RUNTIMES}" PARENT_SCOPE)
			set(DEBUG_RUNTIMES  "${DEBUG_RUNTIMES}" PARENT_SCOPE)
			message(STATUS "Found Intl Dll!")
		else()
			message(STATUS "Couldn't find Intl Dll!")
			unset(INTL_RUNTIME_FILE CACHE)
		endif()
	endif()
endfunction()