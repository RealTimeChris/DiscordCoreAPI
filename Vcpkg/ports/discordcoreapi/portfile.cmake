if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF b42c8ad9fbcd03b2728b6274ab290a902fc40d04
	SHA512 7e7d64e18b0790a1fd2a20b6446b72f28a163c4dee82283a463be5021bfd19d5274fa1d52fca9f6f3dee4feaf43a355919bd026cb4206ecad868bbae3002e792
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
