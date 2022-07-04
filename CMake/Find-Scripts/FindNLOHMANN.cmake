# This file locates the Nlohmann-Json library, using a provided path for searching.
#
# Usage:
#	Set the following directories: NLOHMANN_INCLUDE_DIR
# Where:
#	NLOHMANN_INCLUDE_DIR = The directory containing the public headers.
# What it produces:
#	NLOHMANN::Json
if (UNIX)
	set(LIB_SUFFIX ".a")
	set(LIB_PREFIX "lib")
else()
	set(LIB_SUFFIX ".lib")
	set(LIB_PREFIX "")
endif()
find_file(
	NLOHMANN_JSON_LIBRARY
	NAMES "nlohmann/json.hpp" 
	PATHS "${NLOHMANN_INCLUDE_DIR}"
	NO_DEFAULT_PATH
)
if(EXISTS "${NLOHMANN_JSON_LIBRARY}" AND EXISTS "${NLOHMANN_INCLUDE_DIR}")
	message(STATUS "Found Nlohmann-Json: TRUE")
	add_library(NLOHMANN::Json INTERFACE IMPORTED GLOBAL)
	target_include_directories(NLOHMANN::Json INTERFACE "${NLOHMANN_INCLUDE_DIR}")
else()
	message(FATAL_ERROR "Found Nlohmann-Json: FALSE")
endif()