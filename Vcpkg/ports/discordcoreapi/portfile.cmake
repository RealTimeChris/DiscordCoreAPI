if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 98c811b465039177005c29d224c4c14052918f73
	SHA512 3442f05df677fe49997eab56eebbf16c5b3eaf4bc39a2d832ddb488403b8f6e568e6af7283d06f46c3362082a8ffe0f87584ec9d4fa1b3c4fdfe132b40540f80
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
