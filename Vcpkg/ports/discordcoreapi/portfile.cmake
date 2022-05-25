if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 3d675cac68254678f15f511be54d9f6c95f31e05
	SHA512 5d237b26f1ac62607ba67f5d7709f5da5c76445a0468838dc8cee2ea6d1b831c9d3f692e7ca000c9df9782cb3942d5d6bc585d1336176e2595c3559924c15f61
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
