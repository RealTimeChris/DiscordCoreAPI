if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF a6fbf008c4d51de85723c41850c3bf99830b203e
	SHA512 e2cc475b569a6e929efdc50fc7f6f90aff2d98d3d136340ef69f1d173e719acf119c0e1c6265629fce02f370cd3c499c7d983a68659d49e86a702c4d5b8e6708
	HEAD_REF main
)

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
)

vcpkg_cmake_install()

file(
	INSTALL "${SOURCE_PATH}/License.md"
	DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}"
	RENAME copyright
)
