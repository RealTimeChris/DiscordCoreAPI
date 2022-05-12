if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 367a62a5ef443a31bb2b4b228940f52cdab6cb90
	SHA512 76e0d8888065164d128a6834ac4f261aabd372b1de0d53067bf33fa1a68eea86b636c72ea4a967ea0a5fa32d258892afd1fbfeed933776a0ef6d345d20a66970
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
