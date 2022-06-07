if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF b3094a59773ddbeddcc770fb7455bf59ea344392
	SHA512 f1b23e7717272b5451dc8f399a47ccbf7a570c9f78aa4185fd4e3fb95563429741fbcdcd4227efffb8c000586b2d5ee96320736bc08f1ab11df35466f9ac6950
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
