if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF bf99c6c8a1de9d17b1fb94e59adcb67dc380154d
	SHA512 2a2f9bda0f7518e834653d9a2d7c95602f88e07881002e02c6f3ffca31bce7d81b4b66c3a9a9c870d8a0a5a66dab14a31ea02ae52183be6a97cdbba9538a807e
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
