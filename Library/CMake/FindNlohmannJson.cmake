# find_nlohmann_json(INCLUDE_DIR)
#
# This function locates the Nlohmann-Json, using a provided path for searching.
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
		NAMES "json.hpp" 
		PATHS "${INCLUDE_DIR}" "${INCLUDE_DIR}/nlohmann/" NO_DEFAULT_PATH
	)
	if(NLOHMANN_JSON_LIBRARY)
		message(STATUS "Found Nlohmann-Json library!")
		add_library(NLOHMANN::Json INTERFACE IMPORTED GLOBAL)
		set_target_properties(
			NLOHMANN::Json PROPERTIES
			PUBLIC_HEADER "${INCLUDE_DIR}"
	)
	else()
		message(FATAL_ERROR "Couldn't find Nlohmann-Json!")
		return()
	endif()
endfunction()