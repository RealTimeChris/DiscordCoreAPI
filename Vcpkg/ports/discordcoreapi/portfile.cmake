if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 89976ad97900f3cb135aeaa67429037d306847a5
	SHA512 a7b8532f37f963e3b909ea116b07fd65255678bd8b166b4a74eb21919b4e4b51ca8c037e4fdf8a72886c42df7fe329b3b62f0f6b3b200ca1db066dfaa28bc9aa
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
