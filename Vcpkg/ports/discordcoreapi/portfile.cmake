if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 515336499487b5862a0947c8c44687e144becc4a
	SHA512 26a9aad3d32dcd325b21c86a5cc888b4b3c6db248039b7ce1efa53f724a0a6fe66081425ce42f16c3d869efde6ad000ef5d82a7ffb1ffb91db1be9119d1d2437
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
