if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF f508903a72f090aa78e7d2a8388350ae1a2c12b5
	SHA512 2382fd748da0ade518eff324e600611ab9fa846cdb622bbeb5390b6b97c0fb106c21a52a8605edea72b7262b6e226f303ec92fa66b50b5389ca53be1c7abbf70
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
