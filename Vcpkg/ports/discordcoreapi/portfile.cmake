if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 4afbdc49d125fa616d4397fad1b49ddb72ab1d2b
	SHA512 111b05edbd8b6494b790d7da9f6a936ee1fe96f9915af2c81fc93278b9139f9963abbf9dde8d1abca52fa6be7dea2081fce9eec0f97753b6f8f230af58981785
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
