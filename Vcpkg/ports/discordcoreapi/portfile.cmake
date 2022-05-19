if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 8cbf52115718851b3cfa29c8868a5a6508ed8c11
	SHA512 1938d4b2e9b4f73ee9b24c7dc49f5171cf93b8f337142648295af570588cc8bfc598c41effb1875553d7cc4a2e6179ac2bf18876fb9a39b0b71d1f3cfcc69e7c
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
