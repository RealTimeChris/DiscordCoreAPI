if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 5dc3b8dea11ff4b5267445a70d4f8868bc7a889c
	SHA512 3db3f95d81decbaff5dda07c91d88ce904f89a7a0aebcb5b7039a6852ac2ded6fdaadb0df9d6830b47b06accde6a97f4cbc1a0d606671492e49ad51e9bd62577
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