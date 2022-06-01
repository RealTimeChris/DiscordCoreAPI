if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 9a86ffe02f1b130d0c754d6897ec7606dcfa5a19
	SHA512 d8147fb8832918bd6ab7100aa1a6a2cc5f7c2c3daebb3ec266d19d3051114b5435b96cffff6165d3a2ee3501349f87f7dabf32e5fc04fcb0671bfa9d34e1e5d7
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
