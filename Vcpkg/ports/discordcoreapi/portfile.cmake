if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 51be2fc721a677e2e064fc633afc733e29731aa7
	SHA512 86669c1b39396c8f0d59223e2666d18abb4683f626a7fb62cd40b6c48ff4901486b276471e05c651e6ca8b3120ad41e05c149700302e98abb7e9cb2e4115f7fb
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
