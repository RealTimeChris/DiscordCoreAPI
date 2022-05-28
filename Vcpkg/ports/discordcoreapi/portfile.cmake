if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF de7040ddc13108df0a118ef2e352fa8f5b134e41
	SHA512 d7b381a9af099636c5c53a371965dfa4d84775a06a42c3d33a8c30b154dd5089e98106ae0a38627ae70db9d42b4003bda532668cbe064ce6a5faa11c17087695
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
