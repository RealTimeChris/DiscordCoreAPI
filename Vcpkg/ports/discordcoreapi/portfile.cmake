if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 299ceb49f2e1d496a3644ce01e5cc1595c67482d
	SHA512 7d123746b123d40bccfb2e27ca5b31d22955780175daf22628c370d5e906aaa80b3bfcf0b6f07067c55adf968f3e35da404d76e8e78c09b4997e95026f96529b
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