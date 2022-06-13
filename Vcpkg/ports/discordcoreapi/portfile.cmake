if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 229ce9199b3ac6ce2700de8a4c3e1fa6f59614d0
	SHA512 e99344e8b18a5bc5f3a354637b834ad84601f41ef0395bf799b91f21e83c81873a0d00311bca982288a2fb92e05e1b45e65bd6716cdb78a0f6789f4f6918a4cd
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
