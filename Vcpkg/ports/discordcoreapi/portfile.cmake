if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF ae5d00b60a9f713b08e6d442116f2966bb53bb41
	SHA512 55e6ff4c1edd0e82548875e1b1363c4a8c913ca0100c448e2b1a165546a7825db700cdc625cf50da8ebb8449180d5ac09d50c866aebd177ed252112222d377c2
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