if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF b01208f905893b187bdbfc6514f6b1cd055a9e01
	SHA512 107e7d93112389068a5328dd8725a054dc7d423d8cd63e8b8526f63e51c50c2dd92c1123b83c3ab203bdaad5c4158bf22225f026dc81ee9f78bc18ec1e4536e5
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
