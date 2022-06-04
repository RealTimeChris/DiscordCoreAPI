if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF aa08e632122bd71081b86fbb9b02f0d64b174b7d
	SHA512 60e9e59c9b0187e38a254a1e1dc1694348f00079425340caf75cb4b897b5c8e2f503e27f9f2d14e8c3874f80fbf5eabc8d0ca5f853141a2d4095e448b2455c36
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
