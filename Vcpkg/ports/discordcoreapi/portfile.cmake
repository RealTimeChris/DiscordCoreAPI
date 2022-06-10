if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF dabdd8f9981e1977412effebefef0c90eb57c786
	SHA512 97eefa26e6bd7c26185555d0ce355d26f38f621fcb08ce1ca3a5d650b6baca3bde576e232319d754c2aacafdbd059af858fd456edf01714ac5aba62d8589eb9e
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
