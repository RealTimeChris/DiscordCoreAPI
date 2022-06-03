if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF e334d7ec4972a0ddb75b7a25b47da261f2deb924
	SHA512 b6efff53679aaac49f1eeed140ca8d7e1acde5f373db49059b1a45cc65438db9bc9757853a1bc3e1f00a76f43a357194e6a1ab397c2707883ade76720d0d566b
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
