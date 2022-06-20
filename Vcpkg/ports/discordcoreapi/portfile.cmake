if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF b79f18f0ae8cd609a447e34aef6772e4bdfb528d
	SHA512 9f03b52f43867cb4b4eb6c81a6b8f4bde275f3630cd45c42fe084aafaf501be3c614b7a2874ec1febc4da0f9d74ce7e212487dd85e9be105deb432844cee08c5
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