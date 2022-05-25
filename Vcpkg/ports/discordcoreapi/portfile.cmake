if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF e4158f31de415d748a59ef2298c22a34404696be
	SHA512 8364523ca8ee23538bb5c1e2eccc2df78784067d1c8c21c1d624214815c7d96dd5a9b5207d23b30d3bf598ab31c7bfd9f9cafec314abc0790dc32bd49fd05da1
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
