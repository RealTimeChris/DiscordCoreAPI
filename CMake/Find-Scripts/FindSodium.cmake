#[=======================================================================[.rst:
FindSODIUM
-------

Finds the SODIUM library.

Imported Targets
^^^^^^^^^^^^^^^^

This module provides the following imported targets, if found:

``SODIUM::Sodium``
  The SODIUM library

Result Variables
^^^^^^^^^^^^^^^^

This will define the following variables:

``SODIUM_FOUND``
  True if the system has the SODIUM library.
``SODIUM_INCLUDE_DIRS``
  Include directories needed to use SODIUM.
``SODIUM_LIBRARIES``
  Libraries needed to link to SODIUM.

Cache Variables
^^^^^^^^^^^^^^^

The following cache variables may also be set:

``SODIUM_INCLUDE_DIR``
  The directory containing ``curl.h``.
``SODIUM_ROOT_DIR``
  The root path to the SODIUM libraries.

#]=======================================================================]
include(FindPackageHandleStandardArgs)

find_path(SODIUM_INCLUDE_DIR
  NAMES "sodium/sodium.h"
  PATHS ${SODIUM_INCLUDE_DIR}
)

find_library(
	SODIUM_RELEASE_LIBRARY 
	NAMES "libsodium${LIBRARY_SUFFIX}" 
	PATHS "${SODIUM_ROOT_DIR}" NO_DEFAULT_PATH
)
find_library(
	SODIUM_DEBUG_LIBRARY 
	NAMES "libsodium${LIBRARY_SUFFIX}" 
	PATHS "${SODIUM_ROOT_DIR}" NO_DEFAULT_PATH
)

cmake_path(GET SODIUM_RELEASE_LIBRARY PARENT_PATH SODIUM_RELEASE_FILE_PATH)
find_file(
	SODIUM_RELEASE_DLL
	NAMES "libsodium.dll" "sodium.dll" 
	PATHS "${SODIUM_RELEASE_FILE_PATH}/" "${SODIUM_RELEASE_FILE_PATH}/../bin/"
	NO_DEFAULT_PATH
)
cmake_path(GET SODIUM_DEBUG_LIBRARY PARENT_PATH SODIUM_DEBUG_FILE_PATH)
find_file(
	SODIUM_DEBUG_DLL
	NAMES "libsodium.dll" "sodium.dll" 
	PATHS "${SODIUM_DEBUG_FILE_PATH}/" "${SODIUM_DEBUG_FILE_PATH}/../bin/"
	NO_DEFAULT_PATH
)
if (EXISTS "${SODIUM_RELEASE_DLL}" AND EXISTS "${SODIUM_DEBUG_DLL}")
	find_package_handle_standard_args(
		SODIUM
		FOUND_VAR SODIUM_FOUND
		REQUIRED_VARS
		SODIUM_RELEASE_DLL SODIUM_DEBUG_DLL SODIUM_RELEASE_LIBRARY SODIUM_DEBUG_LIBRARY SODIUM_INCLUDE_DIR 
	)
	if (SODIUM_FOUND)
		set(SODIUM_LIBRARIES "${SODIUM_RELEASE_LIBRARY}" "${SODIUM_DEBUG_LIBRARY}" "${SODIUM_RELEASE_DLL}" "${SODIUM_DEBUG_DLL}")
		set(SODIUM_INCLUDE_DIRS "${SODIUM_INCLUDE_DIR}")
		add_library(SODIUM::Sodium SHARED IMPORTED GLOBAL)
		set_target_properties(
			SODIUM::Sodium PROPERTIES 
			IMPORTED_LOCATION_RELEASE "${SODIUM_RELEASE_DLL}" IMPORTED_LOCATION_DEBUG "${SODIUM_DEBUG_DLL}"
			IMPORTED_IMPLIB_RELEASE "${SODIUM_RELEASE_LIBRARY}" IMPORTED_IMPLIB_DEBUG "${SODIUM_DEBUG_LIBRARY}"
		)
		target_include_directories(SODIUM::Sodium INTERFACE "${INCLUDE_DIR}")
	endif()
else()
	find_package_handle_standard_args(
		SODIUM
		FOUND_VAR SODIUM_FOUND
		REQUIRED_VARS
		SODIUM_RELEASE_LIBRARY SODIUM_DEBUG_LIBRARY SODIUM_INCLUDE_DIR 
	)
	if (SODIUM_FOUND)
		set(SODIUM_LIBRARIES "${SODIUM_RELEASE_LIBRARY}" "${SODIUM_DEBUG_LIBRARY}")
		set(SODIUM_INCLUDE_DIRS "${SODIUM_INCLUDE_DIR}")
		add_library(SODIUM::Sodium STATIC IMPORTED GLOBAL)
		set_target_properties(
			SODIUM::Sodium PROPERTIES 
			IMPORTED_LOCATION_RELEASE "${SODIUM_RELEASE_LIBRARY}" IMPORTED_LOCATION_DEBUG "${SODIUM_DEBUG_LIBRARY}"
		)
		target_include_directories(SODIUM::Sodium INTERFACE "${INCLUDE_DIR}")
		unset(SODIUM_RELEASE_DLL CACHE)
		unset(SODIUM_DEBUG_DLL CACHE)
	endif()
endif()
