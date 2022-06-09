if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 22e88d816c2775dcec3fb91001b81ca4879bc1ba
	SHA512 03380afbbf95502d53742b9549d0095771e7760e6c4f06203d5d0bd123db577c78296e64b14515d7e9fefbe25eddc4cd49d4b5ebf06930fc34a15f3f6d8bdde3
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
