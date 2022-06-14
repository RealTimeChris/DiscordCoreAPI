if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF ac4794c2cfbe8a15e322f30089675fbd69dfb83f
	SHA512 8677cd2a6a63796207be65b94e8d3413741305110e0af41d2d1634ccf4498c9134ee82efdfdb9faaaafd6c86baa83671928f129b2b7539e8e11af1771aa2f61d
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
