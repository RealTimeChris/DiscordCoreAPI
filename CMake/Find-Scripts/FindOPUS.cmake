#[=======================================================================[.rst:
FindOPUS
-------

Finds the OPUS library.

Imported Targets
^^^^^^^^^^^^^^^^

This module provides the following imported targets, if found:

``OPUS::Opus``
  The OPUS library

Result Variables
^^^^^^^^^^^^^^^^

This will define the following variables:

``OPUS_FOUND``
  True if the system has the OPUS library.
``OPUS_INCLUDE_DIRS``
  Include directories needed to use OPUS.
``OPUS_LIBRARIES``
  Libraries needed to link to OPUS.

Cache Variables
^^^^^^^^^^^^^^^

The following cache variables may also be set:

``OPUS_INCLUDE_DIR``
  The directory containing ``opus.h``.
``OPUS_ROOT_DIR``
  The root path to the OPUS libraries.

#]=======================================================================]
include(FindPackageHandleStandardArgs)

find_path(OPUS_INCLUDE_DIR
  NAMES "opus/opus.h"
  PATHS ${OPUS_INCLUDE_DIR}
)

find_library(
	OPUS_RELEASE_LIBRARY
	NAMES "${LIBRARY_PREFIX}opus${LIBRARY_SUFFIX}" 
	PATHS "${OPUS_ROOT_DIR}/Release" "${OPUS_ROOT_DIR}/lib" NO_DEFAULT_PATH
)
find_library(
	OPUS_DEBUG_LIBRARY 
	NAMES "${LIBRARY_PREFIX}opus${LIBRARY_SUFFIX}" 
	PATHS "${OPUS_ROOT_DIR}/Debug" "${OPUS_ROOT_DIR}/debug/lib" NO_DEFAULT_PATH
)

cmake_path(GET OPUS_RELEASE_LIBRARY PARENT_PATH OPUS_RELEASE_FILE_PATH)
find_file(
	OPUS_RELEASE_DLL
	NAMES "opus.dll" "libopus.dll"
	PATHS "${OPUS_RELEASE_FILE_PATH}/" "${OPUS_RELEASE_FILE_PATH}/../bin/"
	NO_DEFAULT_PATH
)
cmake_path(GET OPUS_DEBUG_LIBRARY PARENT_PATH OPUS_DEBUG_FILE_PATH)
find_file(
	OPUS_DEBUG_DLL
	NAMES "opus.dll" "libopus.dll"
	PATHS "${OPUS_DEBUG_FILE_PATH}/" "${OPUS_DEBUG_FILE_PATH}/../bin/"
	NO_DEFAULT_PATH
)
if (EXISTS "${OPUS_RELEASE_DLL}" AND EXISTS "${OPUS_DEBUG_DLL}")
	find_package_handle_standard_args(
		OPUS
		FOUND_VAR OPUS_FOUND
		REQUIRED_VARS
		OPUS_RELEASE_DLL OPUS_DEBUG_DLL OPUS_RELEASE_LIBRARY OPUS_DEBUG_LIBRARY OPUS_INCLUDE_DIR 
	)
	if (OPUS_FOUND)
		set(OPUS_LIBRARIES "${OPUS_RELEASE_LIBRARY}" "${OPUS_DEBUG_LIBRARY}" "${OPUS_RELEASE_DLL}" "${OPUS_DEBUG_DLL}")
		set(OPUS_INCLUDE_DIRS "${OPUS_INCLUDE_DIR}")
		add_library(OPUS::Opus SHARED IMPORTED GLOBAL)
		set_target_properties(
			OPUS::Opus PROPERTIES 
			IMPORTED_LOCATION_RELEASE "${OPUS_RELEASE_DLL}" IMPORTED_LOCATION_DEBUG "${OPUS_DEBUG_DLL}"
			IMPORTED_IMPLIB_RELEASE "${OPUS_RELEASE_LIBRARY}" IMPORTED_IMPLIB_DEBUG "${OPUS_DEBUG_LIBRARY}"
		)
		target_include_directories(OPUS::Opus INTERFACE "${INCLUDE_DIR}")
	endif()
else()
	find_package_handle_standard_args(
		OPUS
		FOUND_VAR OPUS_FOUND
		REQUIRED_VARS
		OPUS_RELEASE_LIBRARY OPUS_DEBUG_LIBRARY OPUS_INCLUDE_DIR 
	)
	if (OPUS_FOUND)
		set(OPUS_LIBRARIES "${OPUS_RELEASE_LIBRARY}" "${OPUS_DEBUG_LIBRARY}")
		set(OPUS_INCLUDE_DIRS "${OPUS_INCLUDE_DIR}")
		add_library(OPUS::Opus STATIC IMPORTED GLOBAL)
		set_target_properties(
			OPUS::Opus PROPERTIES 
			IMPORTED_LOCATION_RELEASE "${OPUS_RELEASE_LIBRARY}" IMPORTED_LOCATION_DEBUG "${OPUS_DEBUG_LIBRARY}"
		)
		target_include_directories(OPUS::Opus INTERFACE "${INCLUDE_DIR}")
		unset(OPUS_RELEASE_DLL CACHE)
		unset(OPUS_DEBUG_DLL CACHE)
	endif()
endif()
