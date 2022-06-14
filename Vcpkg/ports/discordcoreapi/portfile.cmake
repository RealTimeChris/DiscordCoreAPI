if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF c90ad0e14a208a93de07ee54b3ab4a657b4e379b
	SHA512 1884814880ec9607c1d49ddf2c4a3ded8604140261c82aae3380c8f9cfaec838ee235335a143176cca8d87ad9261bd90656016d908dae1980ccada8386862e7f
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
