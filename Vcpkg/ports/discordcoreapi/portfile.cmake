if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF f552e984b4aaa2e8d19165f2753a51c7019bd6bf
	SHA512 6c5982fcf813891ab8c560ace84cc12c7e5526a64731538f6f6f33a892a25f74e8930ecdd4db43d1c7d93302f1d8061fb2f17d78e85271dd9b07ccd74f91f9d0
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
