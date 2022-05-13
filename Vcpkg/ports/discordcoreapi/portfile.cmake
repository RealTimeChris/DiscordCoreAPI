if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 20bce1b85a311b9f253d2828fc4363a8f1a50f62
	SHA512 9e2efb457e302eb9e38e1bfdbdcb5b86330b2b2f7448793f661427e49d8ae64f8be607cb248b022cbef04ed6648a2aa34137c9280f6657110381960abe077021
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
