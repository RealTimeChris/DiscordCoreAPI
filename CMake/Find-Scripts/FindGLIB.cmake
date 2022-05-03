#[=======================================================================[.rst:
FindGLIB
-------

Finds the GLIB library.

Imported Targets
^^^^^^^^^^^^^^^^

This module provides the following imported targets, if found:

``GLIB::Glib``
  The GLIB library

Result Variables
^^^^^^^^^^^^^^^^

This will define the following variables:

``GLIB_FOUND``
  True if the system has the GLIB library.
``GLIB_INCLUDE_DIRS``
  Include directories needed to use GLIB.
``GLIB_LIBRARIES``
  Libraries needed to link to GLIB.

Cache Variables
^^^^^^^^^^^^^^^

The following cache variables may also be set:

``GLIB_INCLUDE_DIR``
  The directory containing ``glib.h``.
``GLIB_ROOT_DIR``
  The root path to the GLIB libraries.

#]=======================================================================]
include(FindPackageHandleStandardArgs)

find_path(GLIB_INCLUDE_DIR
  NAMES "glib/glib.h"
  PATHS ${GLIB_INCLUDE_DIR}
)

find_library(
	GLIB_RELEASE_LIBRARY
	NAMES "${LIBRARY_PREFIX}glib-2.0${LIBRARY_SUFFIX}" 
	PATHS "${GLIB_ROOT_DIR}/lib" "${GLIB_ROOT_DIR}" NO_DEFAULT_PATH
)
find_library(
	GLIB_DEBUG_LIBRARY 
	NAMES "${LIBRARY_PREFIX}glib-2.0${LIBRARY_SUFFIX}" 
	PATHS "${GLIB_ROOT_DIR}/debug/lib" "${GLIB_ROOT_DIR}" NO_DEFAULT_PATH
)

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
	find_package_handle_standard_args(
		GLIB
		FOUND_VAR GLIB_FOUND
		REQUIRED_VARS
		GLIB_RELEASE_DLL GLIB_DEBUG_DLL GLIB_RELEASE_LIBRARY GLIB_DEBUG_LIBRARY GLIB_INCLUDE_DIR 
	)
	if (GLIB_FOUND)
		set(GLIB_LIBRARIES "${GLIB_RELEASE_LIBRARY}" "${GLIB_DEBUG_LIBRARY}" "${GLIB_RELEASE_DLL}" "${GLIB_DEBUG_DLL}")
		set(GLIB_INCLUDE_DIRS "${GLIB_INCLUDE_DIR}")
		add_library(GLIB::Glib SHARED IMPORTED GLOBAL)
		set_target_properties(
			GLIB::Glib PROPERTIES 
			IMPORTED_LOCATION_RELEASE "${GLIB_RELEASE_DLL}" IMPORTED_LOCATION_DEBUG "${GLIB_DEBUG_DLL}"
			IMPORTED_IMPLIB_RELEASE "${GLIB_RELEASE_LIBRARY}" IMPORTED_IMPLIB_DEBUG "${GLIB_DEBUG_LIBRARY}"
		)
		target_include_directories(GLIB::Glib INTERFACE "${INCLUDE_DIR}")
	endif()
else()
	find_package_handle_standard_args(
		GLIB
		FOUND_VAR GLIB_FOUND
		REQUIRED_VARS
		GLIB_RELEASE_LIBRARY GLIB_DEBUG_LIBRARY GLIB_INCLUDE_DIR 
	)
	if (GLIB_FOUND)
		set(GLIB_LIBRARIES "${GLIB_RELEASE_LIBRARY}" "${GLIB_DEBUG_LIBRARY}")
		set(GLIB_INCLUDE_DIRS "${GLIB_INCLUDE_DIR}")
		add_library(GLIB::Glib STATIC IMPORTED GLOBAL)
		set_target_properties(
			GLIB::Glib PROPERTIES 
			IMPORTED_LOCATION_RELEASE "${GLIB_RELEASE_LIBRARY}" IMPORTED_LOCATION_DEBUG "${GLIB_DEBUG_LIBRARY}"
		)
		target_include_directories(GLIB::Glib INTERFACE "${INCLUDE_DIR}")
		unset(GLIB_RELEASE_DLL CACHE)
		unset(GLIB_DEBUG_DLL CACHE)
	endif()
endif()
