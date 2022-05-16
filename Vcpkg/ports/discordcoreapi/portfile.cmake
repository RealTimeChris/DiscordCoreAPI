if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF d50ea20db28dab6118fb2df763752c0119a3a968
	SHA512 f3a59d3c74f1f7d40a304c9dc8759643848d1dfb91810f5ee9d5c2ef280a02c282bd0e8262e5e3a04d561b2e2c054e2a31e64951788b2f55b86a747b6a5846a0
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
