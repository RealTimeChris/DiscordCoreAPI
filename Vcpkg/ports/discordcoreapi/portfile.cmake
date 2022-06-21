if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 871fa8c59b25674201119a7335fa0b25ea504a7a
	SHA512 ea16dc53b905f7357ba0d999fe2e572b2e9bad35057ebf1099c49f4dbe2f646599fd55e5086458fb08c6791e1f524de6e690cdf1ccb7eb5b03e47484138a0e57
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