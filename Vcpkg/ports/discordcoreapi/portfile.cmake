if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 2f76b04ff6e0fb175c54c62c460195f4f793656c
	SHA512 6809d0f6a78d63b6b2166e59e81f5d3dc7ab0c771abca0f38565a6f53dc537e36d60de08cd2012d9aab1ac652cc2c685f19ea26cadab91f2c623a1b46361ae2a
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