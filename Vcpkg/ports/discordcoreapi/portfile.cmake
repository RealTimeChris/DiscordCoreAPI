if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF ef30db5d8a838a13170f411372e7a029a87056e4
	SHA512 8404db5475ee11927cadf0cd15e458e46c64bb39e06ba80aaf8eaffe668f6c61e4bebc06ecdbcbe2c44b46f87463c3945b68322d71a2a402dc773ff9f47c2add
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
