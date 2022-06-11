if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 57b52b8e1f17f66ac27aa3ca9cc810d28828ec30
	SHA512 0f9080bfadc7fbdf45d4fc7694753a8495f049f95641c3d4a0bfc0019e991468f6ab599319260ac0f4b314c71d6f12d6ca67bb0701aaf48e0adbb677321bde3b
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
