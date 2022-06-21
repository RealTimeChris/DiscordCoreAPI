if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 8b4ec96229c970c252d20c60352d62f097c6d490
	SHA512 161e6026872e9df11fb35e9b95ceaf9477f89885722ba3878405846f246ee52d71f9eb05e76c49849d176521ff68cf19f4d2bcd2cd74548e9c3511ba0c44c07e
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