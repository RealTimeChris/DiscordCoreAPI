if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 75b833ee32b26ca18c21867905d235f89a185fb7
	SHA512 b08d2fffa05623ff9e7373ca9364339bd078efbb35584bd076d0d3e43a64dbc8a5276cd4c74ef2f75dd68b8cc9ff8cc11c57f06bf8cc35fdd2e0ad5158b7df47
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