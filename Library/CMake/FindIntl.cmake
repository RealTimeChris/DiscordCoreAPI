# find_intl(ROOT_DIR)
#
# This function locates the Opus library, using a couple of provided paths for searching.
#
# Usage:
#	find_intl(ROOT_DIR)
# Where:
#	ROOT_DIR = The directory containing the library, or library's linker file.
# What it produces:
#	INTL_LIBRARY_RELEASE, INTL_LIBRARY_DEBUG and in the case of SHARED LIBRARIES - 
#	INTL_RUNTIME_RELEASE, and INTL_RUNTIME_DEBUG. Which each points to the respective files of each kind. 
#
function(find_intl ROOT_DIR SHARED)
	find_file(
		INTL_LIBRARY_RELEASE
		NAMES "libintl.dll.a" "libintl.lib" "libintl.a"
		PATHS "${ROOT_DIR}" NO_DEFAULT_PATH
	)
	find_file(
		INTL_LIBRARY_DEBUG 
		NAMES "libintl.dll.a" "libintl.lib" "libintl.a"
		PATHS "${ROOT_DIR}" NO_DEFAULT_PATH
	)
	if(INTL_LIBRARY_RELEASE AND INTL_LIBRARY_DEBUG)
		list(APPEND RELEASE_LIBRARIES "${INTL_LIBRARY_RELEASE}")
		list(APPEND DEBUG_LIBRARIES  "${INTL_LIBRARY_DEBUG}")
		set(RELEASE_LIBRARIES "${RELEASE_LIBRARIES}" PARENT_SCOPE)
		set(DEBUG_LIBRARIES  "${DEBUG_LIBRARIES}" PARENT_SCOPE)
		message(STATUS "Found Intl libraries!")
	else()
		message(STATUS "Couldn't find Intl!")
		return()
	endif()
	if ("${SHARED}")
		cmake_path(GET INTL_LIBRARY_RELEASE PARENT_PATH INTL_RELEASE_FILE_PATH)
		find_file(
			INTL_RUNTIME_RELEASE
			NAMES "libintl-8.dll" "intl-8.dll"
			PATHS "${INTL_RELEASE_FILE_PATH}/" "${INTL_RELEASE_FILE_PATH}/../bin/"
			NO_DEFAULT_PATH
		)
		cmake_path(GET INTL_LIBRARY_DEBUG PARENT_PATH INTL_DEBUG_FILE_PATH)
		find_file(
			INTL_RUNTIME_DEBUG
			NAMES "libintl-8.dll" "intl-8.dll"
			PATHS "${INTL_DEBUG_FILE_PATH}/" "${INTL_DEBUG_FILE_PATH}/../bin/"
			NO_DEFAULT_PATH
		)
		if (INTL_RUNTIME_RELEASE AND INTL_RUNTIME_DEBUG)
			list(APPEND LIBRARY_NAMES "INTL")
			set(LIBRARY_NAMES "${LIBRARY_NAMES}" PARENT_SCOPE)
			list(APPEND RELEASE_RUNTIMES "${INTL_RUNTIME_RELEASE}")
			list(APPEND DEBUG_RUNTIMES  "${INTL_RUNTIME_DEBUG}")
			set(RELEASE_RUNTIMES "${RELEASE_RUNTIMES}" PARENT_SCOPE)
			set(DEBUG_RUNTIMES  "${DEBUG_RUNTIMES}" PARENT_SCOPE)
			message(STATUS "Found Intl Dlls!")
		else()
			list(APPEND LIBRARY_NAMES "INTL")
			set(LIBRARY_NAMES "${LIBRARY_NAMES}" PARENT_SCOPE)
			list(APPEND RELEASE_RUNTIMES "NOTFOUND")
			list(APPEND DEBUG_RUNTIMES  "NOTFOUND")
			set(RELEASE_RUNTIMES "${RELEASE_RUNTIMES}" PARENT_SCOPE)
			set(DEBUG_RUNTIMES  "${DEBUG_RUNTIMES}" PARENT_SCOPE)
			unset(INTL_RUNTIME_RELEASE CACHE)
			unset(INTL_RUNTIME_DEBUG CACHE)
			message(STATUS "Couldn't find Intl Dlls - linking statically!")
		endif()
	endif()
endfunction()