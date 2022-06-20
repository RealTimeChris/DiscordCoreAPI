if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 6562ea63f78700e4ec1d4dd9134fa895336ec071
	SHA512 45272e405f27f14a1a69d9ffdbf71f254a90d01ad56e03fdf0569e8c814064114b1317d177ebff03e06c97d47738b46b5075a73d8d35671bbd7b47dac3888982
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