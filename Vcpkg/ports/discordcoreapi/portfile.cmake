if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 9ab42503248fb0213fa72773f732e6ce0997b0d5
	SHA512 f5491b7e0fb60c0c15f8bbe8521dff0a4db835d40bd6a74137cdc642084e68dcc2c9fb9b92fbd1a4b23cf392fc49a1585a30b8d31732ddd74f27fe09b9f2b070
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
