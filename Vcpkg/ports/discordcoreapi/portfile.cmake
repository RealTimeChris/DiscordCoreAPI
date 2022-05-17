if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 2f17cfdb3f9973d069196a3000a71612561a149c
	SHA512 40d151244826b3b59ccefbb0d7ba003c7ef740191ef2a18155074eb0eedfbbea368b7d48e86815f958656234c45abc750f0cd6e4acd943cd1a80bfa2aaf0a67d
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
