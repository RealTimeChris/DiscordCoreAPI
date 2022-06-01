if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 01af161959a7c17459a7380706662fdcaad5b7d1
	SHA512 b4e00f6c19451c5210e80a79a149177ed8a6538b69c6176dd2a3da8695b1a90e2a087f810e1b6d3c3ec2979ad35405b5b6eb265556ded9781a4a254ddaf148fc
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
