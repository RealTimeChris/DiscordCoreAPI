# This file locates the Glib library, using a couple of provided paths for searching.
#
# Usage:
#	Set the following directories: GLIB_RELEASE_ROOT, GLIB_DEBUG_ROOT, GLIB_INCLUDE_DIR
# Where:
#	GLIB_RELEASE_ROOT = The directory containing the RELEASE version of the library, or library's linker file.
#	GLIB_DEBUG_ROOT = The directory containing the DEBUG version of the library, or library's linker file.
#	GLIB_INCLUDE_DIR = The directory containing the public headers.
# What it produces:
#	GLIB::Glib
find_library(
	GLIB_RELEASE_LIBRARY
	NAMES "${LIB_PREFIX}glib-2.0${LIB_SUFFIX}" 
	PATHS "${GLIB_RELEASE_ROOT}" NO_DEFAULT_PATH
)
find_library(
	GLIB_DEBUG_LIBRARY 
	NAMES "${LIB_PREFIX}glib-2.0${LIB_SUFFIX}" 
	PATHS "${GLIB_DEBUG_ROOT}" NO_DEFAULT_PATH
)
if(EXISTS "${GLIB_RELEASE_LIBRARY}" AND EXISTS "${GLIB_DEBUG_LIBRARY}" AND EXISTS "${GLIB_INCLUDE_DIR}")
	message(STATUS "Found Glib libraries!")
else()
	message(FATAL_ERROR "Couldn't find Glib!")
endif()
cmake_path(GET GLIB_RELEASE_LIBRARY PARENT_PATH GLIB_RELEASE_FILE_PATH)
find_file(
	GLIB_RELEASE_DLL
	NAMES "libglib-2.0-0.dll" "glib-2.0-0.dll" 
	PATHS "${GLIB_RELEASE_FILE_PATH}/" "${GLIB_RELEASE_FILE_PATH}/../bin/"
	NO_DEFAULT_PATH
)
cmake_path(GET GLIB_DEBUG_LIBRARY PARENT_PATH GLIB_DEBUG_FILE_PATH)
find_file(
	GLIB_DEBUG_DLL
	NAMES "libglib-2.0-0.dll" "glib-2.0-0.dll"
	PATHS "${GLIB_DEBUG_FILE_PATH}/" "${GLIB_DEBUG_FILE_PATH}/../bin/"
	NO_DEFAULT_PATH
)
if (EXISTS "${GLIB_RELEASE_DLL}" AND EXISTS "${GLIB_DEBUG_DLL}")
	add_library(GLIB::Glib SHARED IMPORTED GLOBAL)
	set_target_properties(
		GLIB::Glib PROPERTIES 
		IMPORTED_LOCATION_RELEASE "${GLIB_RELEASE_DLL}" IMPORTED_LOCATION_DEBUG "${GLIB_DEBUG_DLL}"
		IMPORTED_IMPLIB_RELEASE "${GLIB_RELEASE_LIBRARY}" IMPORTED_IMPLIB_DEBUG "${GLIB_DEBUG_LIBRARY}"
	)
	target_include_directories(GLIB::Glib INTERFACE "$<INSTALL_INTERFACE:${GLIB_INCLUDE_DIR}>")
	message(STATUS "Found Glib Dlls!")
else()
	add_library(GLIB::Glib STATIC IMPORTED GLOBAL)
	set_target_properties(
		GLIB::Glib PROPERTIES 
		IMPORTED_LOCATION_RELEASE "${GLIB_RELEASE_LIBRARY}" IMPORTED_LOCATION_DEBUG "${GLIB_DEBUG_LIBRARY}"
	)
	target_include_directories(GLIB::Glib INTERFACE "$<INSTALL_INTERFACE:${GLIB_INCLUDE_DIR}>")
	unset(GLIB_RELEASE_DLL CACHE)
	unset(GLIB_DEBUG_DLL CACHE)
	message(STATUS "Couldn't find Glib Dlls - linking statically!")
endif()	
# This file locates the Intl library, using a couple of provided paths for searching.
#
# Usage:
#	Set the following directories: GLIB_RELEASE_ROOT, GLIB_DEBUG_ROOT, GLIB_INCLUDE_DIR
# Where:
#	GLIB_RELEASE_ROOT = The directory containing the RELEASE version of the library, or library's linker file.
#	GLIB_DEBUG_ROOT = The directory containing the DEBUG version of the library, or library's linker file.
#	GLIB_INCLUDE_DIR = The directory containing the public headers.
# What it produces:
#	INTL::Intl
find_file(
	INTL_RELEASE_LIBRARY
	NAMES "libintl.dll.a" "libintl.lib" "libintl.a" "intl.lib" "intl.a"
	PATHS "${GLIB_RELEASE_ROOT}" NO_DEFAULT_PATH
)
find_file(
	INTL_DEBUG_LIBRARY 
	NAMES "libintl.dll.a" "libintl.lib" "libintl.a" "intl.lib" "intl.a"
	PATHS "${GLIB_DEBUG_ROOT}" NO_DEFAULT_PATH
)
if(EXISTS "${INTL_RELEASE_LIBRARY}" AND EXISTS "${INTL_DEBUG_LIBRARY}" AND EXISTS "${GLIB_INCLUDE_DIR}")
	message(STATUS "Found Intl libraries!")
else()
	message(STATUS "Couldn't find Intl!")
	unset(INTL_RELEASE_LIBRARY CACHE)
	unset(INTL_DEBUG_LIBRARY CACHE)
	return()
endif()
cmake_path(GET INTL_RELEASE_LIBRARY PARENT_PATH INTL_RELEASE_FILE_PATH)
find_file(
	INTL_RELEASE_DLL
	NAMES "libintl-8.dll" "intl-8.dll"
	PATHS "${INTL_RELEASE_FILE_PATH}/" "${INTL_RELEASE_FILE_PATH}/../bin/"
	NO_DEFAULT_PATH
)
cmake_path(GET INTL_DEBUG_LIBRARY PARENT_PATH INTL_DEBUG_FILE_PATH)
find_file(
	INTL_DEBUG_DLL
	NAMES "libintl-8.dll" "intl-8.dll"
	PATHS "${INTL_DEBUG_FILE_PATH}/" "${INTL_DEBUG_FILE_PATH}/../bin/"
	NO_DEFAULT_PATH
)
if (EXISTS "${INTL_RELEASE_DLL}" AND EXISTS "${INTL_DEBUG_DLL}")
	add_library(INTL::Intl SHARED IMPORTED GLOBAL)
	set_target_properties(
		INTL::Intl PROPERTIES 
		IMPORTED_LOCATION_RELEASE "${INTL_RELEASE_DLL}" IMPORTED_LOCATION_DEBUG "${INTL_DEBUG_DLL}"
		IMPORTED_IMPLIB_RELEASE "${INTL_RELEASE_LIBRARY}" IMPORTED_IMPLIB_DEBUG "${INTL_DEBUG_LIBRARY}"
	)
	target_include_directories(INTL::Intl INTERFACE "${GLIB_INCLUDE_DIR}")
	message(STATUS "Found Intl Dlls!")
else()
	add_library(INTL::Intl STATIC IMPORTED GLOBAL)
	set_target_properties(
		INTL::Intl PROPERTIES 
		IMPORTED_LOCATION_RELEASE "${INTL_RELEASE_LIBRARY}" IMPORTED_LOCATION_DEBUG "${INTL_DEBUG_LIBRARY}"
	)
	target_include_directories(INTL::Intl INTERFACE "${GLIB_INCLUDE_DIR}")
	unset(INTL_RELEASE_DLL CACHE)
	unset(INTL_DEBUG_DLL CACHE)
	message(STATUS "Couldn't find Intl Dlls - linking statically!")
endif()	