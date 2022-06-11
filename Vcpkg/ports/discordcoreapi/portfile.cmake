if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 56ed3d47db7eda87fc5c849584edf6b45d12855e
	SHA512 9e84bff44cfa9969ae6ead5c9b97be89fde6e42710b4ac3407f17b56425bcfb82b9c4506b717cea5aae698edc17705883c10e64830ef86408f05bab191eff539
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
