if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF edfacecffb1547bd11785a45883c60bbec9b48c9
	SHA512 2c465d6e4a465954f367d08345daa2f773e0e591f7ee0e29f1377573a819cc5fcfb36f28a58068479f7cc1e671c9c918e2daed3973de5621c634995002c6b4fa
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
