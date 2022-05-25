if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 642b5fe1a4ef105acc6d0a29f424b7f20e943515
	SHA512 b0c72783bae0e28b6ef0d0b032e57788975abe1ae13d9cf4d41b6e221372361e6f0c9da16e9e1564c739686f0bda31d3f8ffc23443f272fa05ad048a502b0b8f
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
