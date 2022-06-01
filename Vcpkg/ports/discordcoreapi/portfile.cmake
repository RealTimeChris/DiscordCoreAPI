if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF f9339c52ccaddd87aea2326e8205763b5c6bb904
	SHA512 2a2358b1e25071496ffa03fd33578caa8c766baedb76833154aa8ca3cb29a60c6a1907c67287ee68300f78ef6604f80aa43268f7f9f1f9cd874f47eac51de3a2
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
