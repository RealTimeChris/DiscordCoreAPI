if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 781b145c48c041bfe0530a888c259808d2528f83
	SHA512 3cb0e385d5aa67fdf6e0fc43cd77db4c48eae917e6e3e08a241cb1f32cfe0c5b3d18a5aa0c97089d68eea4ee2ec84258e322bda7a0aa953b6c2c0d55ccab5384
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