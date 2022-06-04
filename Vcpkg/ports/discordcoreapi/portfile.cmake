if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF c317233429d3a919af78eba2f254392a3d3c9fb7
	SHA512 cd8017efb8bd032c28273dc56452c5aa2368e9fd27adf2f1980066ac1d099eed076517a6a76516561fd76b35854d5f98bbd180a31bc8bd25792da63a8e8d92f8
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
