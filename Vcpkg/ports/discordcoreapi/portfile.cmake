if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 3c00267529e10e9d1f46723f7b758e224e11dac9
	SHA512 0e4729f0bf936a3032b002c5182ae4d42d62ee3c504352a5d6cfbb8b010d545815765326bb4fc896bc720e6a0179224dc0ac0e8ad1b09ff5420c74ae524143c4
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
