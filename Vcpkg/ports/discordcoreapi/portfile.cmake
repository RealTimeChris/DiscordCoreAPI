if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF ac8b86ca99cca728a24a5de6c7884062b2fa9810
	SHA512 8dfbdfb62ddcb4bb3adab1b2efb1776c95f2cd58a9af407313828d91f0e3572e5eea5b9932e5f93ed7d0a73b95cb0e4391ae3d5dc40682c46a80741a9d4e46f3
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
