if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 6053ba59325f6db4464a69612a795c324c94726c
	SHA512 5c2fc593308abedf11b2553002073a7f3a713b5cf69234dbea072ab28274081a38fff0724b8ada7dbaede990eaadf2c38ec8f9b39ef5f46a5db2fec35ee30f70
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