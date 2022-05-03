#[=======================================================================[.rst:
FindNLOHMANNJSON
-------

Finds the NLOHMANN library.

Imported Targets
^^^^^^^^^^^^^^^^

This module provides the following imported targets, if found:

``NLOHMANN::Json``
  The NLOHMANN library

Result Variables
^^^^^^^^^^^^^^^^

This will define the following variables:

``NLOHMANNJSON_FOUND``
  True if the system has the NLOHMANN library.
``NLOHMANNJSON_INCLUDE_DIRS``
  Libraries needed to link to NLOHMANN.

Cache Variables
^^^^^^^^^^^^^^^

The following cache variables may also be set:

``NLOHMANNJSON_INCUDE_DIR``
  The root path to the NLOHMANN headers.

#]=======================================================================]
include(FindPackageHandleStandardArgs)

find_path(
	NLOHMANN_JSON_LIBRARY
	NAMES "nlohmann/json.hpp" 
	PATHS "${NLOHMANNJSON_INCLUDE_DIR}" NO_DEFAULT_PATH
)

if(EXISTS "${NLOHMANN_JSON_LIBRARY}")
	find_package_handle_standard_args(
		NLOHMANNJSON
		FOUND_VAR NLOHMANNJSON_FOUND
		REQUIRED_VARS
		NLOHMANN_JSON_LIBRARY
	)
	set(NLOHMANNJSON_INCLUDE_DIRS "${NLOHMANN_JSON_LIBRARY}")
	add_library(NLOHMANN::Json INTERFACE IMPORTED GLOBAL)
	target_include_directories(NLOHMANN::Json INTERFACE "${NLOHMANNJSON_INCLUDE_DIR}")
endif()
