if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 2724cbeaabad380aca96a7a4a7a169cb7cbba403
	SHA512 9fb8b449ce9c120af7c405c341531201e537d0ecefe4c09d4dd68e4a5dad383b9b77bb0ad6e01e3b230a758e6426d0965f33f579d0a3eede68f6b9eb694b3139
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