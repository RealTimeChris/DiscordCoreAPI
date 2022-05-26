if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 3bd5a8f7f2295f416b9397fb527efc1ed4b7d3ae
	SHA512 6c739197382c03fc68da31384e98891bec9df5e45c4afe363b059215116351bfdcc8ee5b4309499ee7a033484b6285e5d97dcf572a63b438c3c92d55443a82cb
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
