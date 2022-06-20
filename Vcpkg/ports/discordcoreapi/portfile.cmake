if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 5ff7497a60a81a39a7d87233d65761657fb433c8
	SHA512 2d2e6e4a1e24364e21e1784015b4d1a4a90bae773a33addd54647b49fb97b9f367b8196211b5e6bdc8db8bfe2951f7e3a7d60b7ef658a133af416928a73b89d5
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