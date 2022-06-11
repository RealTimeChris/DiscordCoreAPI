if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 6e7d1bd31f9d6c54e702a0087fe5ead00a0a8b63
	SHA512 7f6b95e6164eb7d9977a8f781ec7f3a895098378838759aa89f44c71abdc826fc6909c61e778836334a9d31bcf2579cda7ff906799c3a37fa4e31169f914748c
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
