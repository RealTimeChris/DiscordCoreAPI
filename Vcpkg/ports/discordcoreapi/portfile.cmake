if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 7ca1da8b673e3a96bf2c86d03abe00d3036a8d21
	SHA512 ea0bab13238fb820190d501c23b09dff34d1cbfde1d97368d4d65fad9299fe58fa09e1062bd8805742e8987c724908f71a53e599ce34002bd91d7f009a5af5ac
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