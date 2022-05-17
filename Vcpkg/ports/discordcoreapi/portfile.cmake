if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 207747bb86cdadda25d69bfcefd2b77b297ab6ec
	SHA512 61b4ebd654fbb28a029a9491232c38a35f03d2f6f7f6c3a528c65f5509a89e49595a6dd2c6e478dc9a35b9bfdb1c2b2fb80099a71f01f78adb5993668cc8efd6
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
