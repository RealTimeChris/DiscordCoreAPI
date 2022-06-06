if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 7573939bec1e90f42009b6e9c349adf5cc022eba
	SHA512 d9db8664c0e1c81f261586de2cd3f52b515c0193f85cd59ed54fa7ce9f1b92af1c6dd04820d8d5a028ce81982d25299f4ed34eb1bdd9bee284987200e6ac50c3
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
