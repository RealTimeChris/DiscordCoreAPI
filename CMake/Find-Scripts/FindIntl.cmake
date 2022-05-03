#[=======================================================================[.rst:
FindINTL
-------

Finds the INTL library.

Imported Targets
^^^^^^^^^^^^^^^^

This module provides the following imported targets, if found:

``INTL::Intl``
  The INTL library

Result Variables
^^^^^^^^^^^^^^^^

This will define the following variables:

``INTL_FOUND``
  True if the system has the INTL library.
``INTL_LIBRARIES``
  Libraries needed to link to INTL.

Cache Variables
^^^^^^^^^^^^^^^

The following cache variables may also be set:

``INTL_ROOT_DIR``
  The root path to the INTL libraries.

#]=======================================================================]
include(FindPackageHandleStandardArgs)

find_file(
	INTL_RELEASE_LIBRARY
	NAMES "libintl.dll.a" "libintl.lib" "libintl.a" "intl.lib" "intl.a"
	PATHS "${INTL_ROOT_DIR}/lib" "${INTL_ROOT_DIR}/bin" NO_DEFAULT_PATH
)
find_file(
	INTL_DEBUG_LIBRARY 
	NAMES "libintl.dll.a" "libintl.lib" "libintl.a" "intl.lib" "intl.a"
	PATHS "${INTL_ROOT_DIR}/lib" "${INTL_ROOT_DIR}/bin" NO_DEFAULT_PATH
)

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
	find_package_handle_standard_args(
		INTL
		FOUND_VAR INTL_FOUND
		REQUIRED_VARS
		INTL_RELEASE_DLL INTL_DEBUG_DLL INTL_RELEASE_LIBRARY INTL_DEBUG_LIBRARY
	)
	if (INTL_FOUND)
		set(INTL_LIBRARIES "${INTL_RELEASE_LIBRARY}" "${INTL_DEBUG_LIBRARY}" "${INTL_RELEASE_DLL}" "${INTL_DEBUG_DLL}")
		set(INTL_INCLUDE_DIRS "${INTL_INCLUDE_DIR}")
		add_library(INTL::Intl SHARED IMPORTED GLOBAL)
		set_target_properties(
			INTL::Intl PROPERTIES 
			IMPORTED_LOCATION_RELEASE "${INTL_RELEASE_DLL}" IMPORTED_LOCATION_DEBUG "${INTL_DEBUG_DLL}"
			IMPORTED_IMPLIB_RELEASE "${INTL_RELEASE_LIBRARY}" IMPORTED_IMPLIB_DEBUG "${INTL_DEBUG_LIBRARY}"
		)
		target_include_directories(INTL::Intl INTERFACE "${INCLUDE_DIR}")
	endif()
else()
	find_package_handle_standard_args(
		INTL
		FOUND_VAR INTL_FOUND
		REQUIRED_VARS
		INTL_RELEASE_LIBRARY INTL_DEBUG_LIBRARY
	)
	if (INTL_FOUND)
		set(INTL_LIBRARIES "${INTL_RELEASE_LIBRARY}" "${INTL_DEBUG_LIBRARY}")
		set(INTL_INCLUDE_DIRS "${INTL_INCLUDE_DIR}")
		add_library(INTL::Intl STATIC IMPORTED GLOBAL)
		set_target_properties(
			INTL::Intl PROPERTIES 
			IMPORTED_LOCATION_RELEASE "${INTL_RELEASE_LIBRARY}" IMPORTED_LOCATION_DEBUG "${INTL_DEBUG_LIBRARY}"
		)
		target_include_directories(INTL::Intl INTERFACE "${INCLUDE_DIR}")
		unset(INTL_RELEASE_DLL CACHE)
		unset(INTL_DEBUG_DLL CACHE)
	endif()
endif()
