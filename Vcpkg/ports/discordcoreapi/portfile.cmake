if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF acf3e8248b02bb402b8fdae05c7d34b393653f5e
	SHA512 77b26f584610f6d541ed6eb9817d892c847982d7426887ee0e8b36272b2a3e06998a5ee3290bd7371dc87d5d4c7fdc34c89ff3d1f98740cf6f3f32e41df8d822
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