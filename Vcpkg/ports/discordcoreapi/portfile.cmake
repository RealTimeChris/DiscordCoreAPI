if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF c681894ecaa514382f0c7cb68d6d9fbe06cb4ee4
	SHA512 5a5f04e01cedf20343c8669bc2d784afdb48622064d0c923c2ec1cbbcd10e9206bbbb61d0d6a75293d9dcfa8710c13ea6243d13f8149f10cda25e829088fa00c
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
