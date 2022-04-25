# find_nlohmann_json(INCLUDE_DIR)
#
# This function locates the Nlohmann-Json library, using a provided path for searching.
#
# Usage:
#	find_nlohmann_json(INCLUDE_DIR)
# Where:
#	INCLUDE_DIR = The directory containing the public headers.
# What it produces:
#	NLOHMANN::Json
#
function(find_nlohmann_json INCLUDE_DIR)
	find_file(
		NLOHMANN_JSON_LIBRARY
		NAMES "nlohmann/json.hpp" 
		PATHS "${INCLUDE_DIR}" NO_DEFAULT_PATH
	)
	cmake_path(GET NLOHMANN_JSON_LIBRARY PARENT_PATH INCLUDE_DIR_NEW)
	if(EXISTS "${NLOHMANN_JSON_LIBRARY}" AND EXISTS "${INCLUDE_DIR}")
		message(STATUS "Found Nlohmann-Json library!")
		add_library(NLOHMANN::Json INTERFACE IMPORTED GLOBAL)
		set_target_properties(
			NLOHMANN::Json PROPERTIES
			PUBLIC_HEADER "${INCLUDE_DIR}"
			INTERFACE_INCLUDE_DIRECTORIES "${INCLUDE_DIR}"
		)
		target_include_directories(NLOHMANN::Json INTERFACE "${INCLUDE_DIR_NEW}")
	else()
		message(FATAL_ERROR "Couldn't find Nlohmann-Json!")
		return()
	endif()
endfunction()