if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 653805f403b0cf3b28b933ddc40e9ff4f00194ab
	SHA512 b2bb13dc9bb09eff7f367870deaa48f9e405dab3065bcef64a6bdeed069f5fdbc6dc6e6514fd64b699cdeae18c7474eec0f07c04dbc1768156c1350e53c5eb7d
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
