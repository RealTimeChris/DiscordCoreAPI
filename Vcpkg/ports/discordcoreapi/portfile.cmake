if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 439a0fe5530aa84700041d93095319547e40089e
	SHA512 3f35dd6cac9b6586b1f10ff1a594d03c58d7d33267e0d34c3cd42c4ab812dcf724a9cff8c23132ae45746f3705e308b0fad19d09cafc24ab8bf2bf4dd2d0d266
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
