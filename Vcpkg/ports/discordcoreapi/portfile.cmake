if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 1f88176047f2633fd15f19591c97e675b8258d2b
	SHA512 1aaf47d645366588c51aa71aa744348c2060eb33aadc9ccd0fe4a45a089979c4bae0d77dfab85d4e2d0530dddbca907695da983862ee5844d9160e0b87abeddf
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
