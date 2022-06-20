if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF bf305cd344b5e1dbb1e445a426e6c64cde777b44
	SHA512 f8d880aad2efc6977ba3ba4eeedc9fbbd8f0c7da259ea4faf8fb357fb0b8a8a3581e0b051db33f3e6f2a3ce2319e6bab827ad17a5df28daa6d0668a68431ad17
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