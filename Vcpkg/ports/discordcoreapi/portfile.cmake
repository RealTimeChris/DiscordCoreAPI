if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 61c56540269807fcae499349d47b5bfcd4452fd2
	SHA512 4e9c30e43110b06a974b58e657f8ff334c92a7879c8a0a7ae97cf033a064b23b493a829be9b03f117aa1ec460ce03d6f8d0038cd4f6174edd4aef4e793e308a1
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
