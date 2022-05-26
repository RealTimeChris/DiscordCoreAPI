if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF ccae5e583f0d1016488b651b9321831ec0fb372e
	SHA512 c5c7fe7e058e23660f98f522c4b1c7af2c77bae9bac8b1f802705d719409834a7bc0c4e620c77ccde794e948e5a7497d0817c52a6023237406dc2fdd6ccf8c94
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
