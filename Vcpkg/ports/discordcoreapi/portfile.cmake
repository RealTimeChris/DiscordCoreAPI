if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF a354cb96d3fd7757441e5cdaf900e283633379f8
	SHA512 150a64b2ed0183dd812f5d100939302e7d94cfc9e37d6587df8149cb015384e3ebc4ae4cbeb4f241830f88cdb7b8bdc9bd2cdf7ec017a31bad16ad61f6138ed2
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