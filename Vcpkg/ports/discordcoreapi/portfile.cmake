if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF f84606e56114513d83164275269c6b4664cac8b7
	SHA512 834997cedad4a9f226e04f85443bd751e97afb1767f5d87ca41212afca188d4b16e8fff7b859dcb21da35a4126e524afb8e50ba6ed1e7733555c4c9d40d1f6e6
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
