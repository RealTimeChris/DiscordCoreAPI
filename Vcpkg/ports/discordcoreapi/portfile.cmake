if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF ec7248f146b0a0719fbc72e2ab15661d65ea4233
	SHA512 2d5daa7602a71a9d800ccb3ea983045ba05f84c9115daccf25b42685981da6d2aa0440bdb3c1b608d9171abeadc6e6f717493586e1ef239cf4dd9814718981bd
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
