if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF c51718e6b7cb9f5dcf4f82bc34f02a14068e6b41
	SHA512 2a2fe231e74842e684f317d25a4e789295e2c41ebf7aabbb334fc00b8cf9608e261e7262a5971b5d6650b82e2e1de2fbd10c5b330ea70060a36c40cde22cb102
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
