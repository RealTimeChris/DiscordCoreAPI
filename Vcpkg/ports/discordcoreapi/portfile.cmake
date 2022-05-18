if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF f767fba7ec7a068e0c9322269886e7567d64de7c
	SHA512 bcd2c074134d4585275d965a10bb41367a2fb72918590480a32a652287c7d86f10d1a4e1cea26dc5a48ae7dff29dd6baf75cf8e5d122555b06bfb12671a8bce3
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
