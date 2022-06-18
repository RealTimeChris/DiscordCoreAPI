if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 96ba97a9bd5330916020ee2e25d8c61d2f3ab361
	SHA512 2310df245efe189b5e6ad7a0f8d6c56f0fda428290485e74138bdb9a3cc2577193fd2d3ca934153513ce030b478e0c4ab3944a8c76781223903e066a01b14281
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