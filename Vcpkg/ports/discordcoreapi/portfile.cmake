if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 3f35af299db35e8c3506af51e4bcc7ade96adb90
	SHA512 342c1dafb0e168f2c2c5a90887b50f55508d1802695f2a8d2f9efea9ec3bde56959c806f51ec7969844c8848b7331d538214646ca8de7b1ccdd381a5fb203056
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
