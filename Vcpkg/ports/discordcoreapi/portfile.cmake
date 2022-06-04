if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 8c3cac57b94687e02dc70cf4a21658ac60458d1f
	SHA512 b168cbe47112b11b7e5c4945dd4022c3abab9cd3a1dba54df9583b5c9eddc71b37a4257a6ba248a8768bc95fc6156571b255402daf46b250c80813ea8194af4b
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
