# collect_relative_path(INPUT_PATH)
#
# This function collects the relative path to the input path, in relation to the CMAKE_INSTALL_PREFIX.
#
# Usage:
#	collect_relative_path(INPUT_PATH)
# What it produces:
#	OUTPUT_RELATIVE_PATH - Which is a path relative to the CMAKE_INSTALL_PREFIX.
#
function(collect_relative_path INPUT_PATH)
	cmake_path(RELATIVE_PATH INPUT_PATH BASE_DIRECTORY "${CMAKE_INSTALL_PREFIX}" OUTPUT_VARIABLE OUTPUT_RELATIVE_PATH)
	set(OUTPUT_RELATIVE_PATH "${OUTPUT_RELATIVE_PATH}" PARENT_SCOPE)
endfunction()