if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 095b70ef15952214ecee7ba5d83212f24addc514
	SHA512 8e4d9203374ee81f90a64a5197638f5a46596a6ce3f6780e1c96e046e0f11676f04234fc2537348c57ec8510337af054fabed6c1b3886ffe37b6ecd9c04f7ca0
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
