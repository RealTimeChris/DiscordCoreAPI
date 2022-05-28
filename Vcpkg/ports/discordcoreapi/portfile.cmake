if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF bce6381ad2e9d790cca18f2e1ee9d07e0d095cb8
	SHA512 eb9e6c0d1ef789c69b2b87b5f7bfa52edc0db440d32aaf938c86f9f4a84e29c43acb0288e346818fcdaa8bb59ef842cf113879c2e006b635d86ebfd11695912a
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
