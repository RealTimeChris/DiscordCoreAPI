if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF ef3a23e6aa6db0e5707b93e9e688c0f665810cc6
	SHA512 88aab2c357d4b97f67bec6fe0ead86dc0b4d9cb4541dd5397643da0c0e2e033e4e10629d51386791b3c1ad8568d3809a7c41a88aeb4376241554e2ee00257a2a
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
