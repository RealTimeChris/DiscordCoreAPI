if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF ec3944eeaa27d2d81ba1e9491f794eeaf10b45ca
	SHA512 404fe581c130096ebd46173b2c2d03867c77832e7ba567faa9b9eb2834f79d66fa745369b7a33bbbc8d3bcc6d9c4e54541ba4d2f249c570b2fd01e7c0f8edfd5
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
