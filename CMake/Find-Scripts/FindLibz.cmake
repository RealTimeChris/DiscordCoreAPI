#[=======================================================================[.rst:
FindLIBZ
-------

Finds the LIBZ library.

Imported Targets
^^^^^^^^^^^^^^^^

This module provides the following imported targets, if found:

``LIBZ::Libz``
  The LIBZ library

Result Variables
^^^^^^^^^^^^^^^^

This will define the following variables:

``LIBZ_FOUND``
  True if the system has the LIBZ library.
``LIBZ_INCLUDE_DIRS``
  Include directories needed to use LIBZ.
``LIBZ_LIBRARIES``
  Libraries needed to link to LIBZ.

Cache Variables
^^^^^^^^^^^^^^^

The following cache variables may also be set:

``LIBZ_INCLUDE_DIR``
  The directory containing ``curl.h``.
``LIBZ_ROOT_DIR``
  The root path to the LIBZ libraries.

#]=======================================================================]
include(FindPackageHandleStandardArgs)

if (UNIX)

	find_path(LIBZ_INCLUDE_DIR
	  NAMES "libz/libz.h"
	  PATHS ${LIBZ_INCLUDE_DIR}
	)

	find_library(
		LIBZ_RELEASE_LIBRARY
		NAMES "libz.a" 
		PATHS "${RELEASE_ROOT_DIR}/lib" NO_DEFAULT_PATH
	)
	find_library(
		LIBZ_DEBUG_LIBRARY 
		NAMES "libz.a" 
		PATHS "${DEBUG_ROOT_DIR}/debug/lib" NO_DEFAULT_PATH
	)
	if(EXISTS "${LIBZ_RELEASE_LIBRARY}" AND EXISTS "${LIBZ_DEBUG_LIBRARY}" AND EXISTS "${INCLUDE_DIR}")
		find_package_handle_standard_args(
			LIBZ
			FOUND_VAR LIBZ_FOUND
			REQUIRED_VARS
			LIBZ_RELEASE_LIBRARY LIBZ_DEBUG_LIBRARY LIBZ_INCLUDE_DIR 
		)
		if (LIBZ_FOUND)
			set(LIBZ_LIBRARIES "${LIBZ_RELEASE_LIBRARY}" "${LIBZ_DEBUG_LIBRARY}")
			set(LIBZ_INCLUDE_DIRS "${LIBZ_INCLUDE_DIR}")
			add_library(LIBZ::Libz STATIC IMPORTED GLOBAL)
			set_target_properties(
			LIBZ::Libz PROPERTIES 
			IMPORTED_LOCATION_RELEASE "${LIBZ_RELEASE_LIBRARY}" IMPORTED_LOCATION_DEBUG "${LIBZ_DEBUG_LIBRARY}"
		)
			target_include_directories(LIBZ::Libz INTERFACE "${INCLUDE_DIR}")
		endif()
	endif()
endif()
