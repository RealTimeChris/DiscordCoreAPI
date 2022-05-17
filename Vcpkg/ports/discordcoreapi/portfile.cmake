if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 1c937ea01eedea6871743b4f531d47c40f3c248b
	SHA512 e2366ed0157c0a69aeec3fafaeee1ddc30696dfe7c3fd7aea6e3375932b77722248e83c8c243958fb96f9a7b023dab99ca0ecd2359c12cd333725e477bcc9d10
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
