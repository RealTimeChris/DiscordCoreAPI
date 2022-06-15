if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF e494886a8bfc02e6c348b6d26103efb1f7e1140b
	SHA512 aabb4f2753ff2a97d153f1a0dac5e6fcd18477b71ea4839651edffcffce32b6112c85c814438558ba2eec5230c76252c543d2cf7cc6a46f6be88a0183c8bf77e
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