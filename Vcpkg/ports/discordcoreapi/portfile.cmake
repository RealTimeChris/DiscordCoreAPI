if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF d6aa8f35076f58a650c221e9f235f0b50fd77f89
	SHA512 1a40edb4ec277b2a3ef9977f21a790232e87fbabd913fe34a37efba3d8071194f7fe8bd890f708f54852fab8d844be05f9b0a0722b5eebcaa4de2e532005b8f8
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