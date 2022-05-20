if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 755a9f08ca6b3cdbb9f64dca5dce8905f8b67200
	SHA512 565f310047ec1d8bb2ca59f0a3e329e11f7bfb75067c86ae5477b8bd2627cc8dcd37f39939e4f4de3ef1617a3be4fe80461bb30706b81e2aca1389738895bbeb
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
