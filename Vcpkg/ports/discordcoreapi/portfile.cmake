if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 0e7b1d598ddcb6d7d9aa44bc9d223ccaecd187f9
	SHA512 4059b5d7f22e8f577cd2fc11fdd480ae1aaa752cd661a67e51b8c65107c4199e0460a680c3dbb1ab54084642f3cb450523f1013527ccd986d26a47102a920cb0
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