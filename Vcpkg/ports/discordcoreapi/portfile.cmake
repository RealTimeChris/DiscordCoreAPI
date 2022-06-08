if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF dad9febf3df6fec59f369362eb339e512a87ac08
	SHA512 55310f52fbce033d23b84c7f57d333524332b8271935b697f4b5dd3ed88dd1d2e1803cfb1780724f0dfbbb04e066f59f3863e23b83a42c47429029b0fd95ee5a
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
