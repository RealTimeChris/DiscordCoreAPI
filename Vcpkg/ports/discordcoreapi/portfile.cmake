if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF eba0999ff3cbec6fc47f9bc400ac9c9230222b6b
	SHA512 b54620bcf99264f73d3d57f89549e0fdcd12fa05936d78e13391421acdacbb819d0b1709aafe5d23459d7af22fdfc8ddf9c58e5d1978ed7659380ec98954ebef
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
