if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF ccfa861a6051b46686d8b22b7acf4b70ce947cde
	SHA512 0f19448ffc91ad8584de3ec4b97328240d403266c42667484d24c2dc2c85a9a93dd6f4ab35e832c44e30b97f496c787363be5cdb4b01a174e6b13c3c7cb054f2
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
