if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 4a5c88112e2625873a5782f8a4b8b2702e24cf51
	SHA512 897e5ec18407300444b71707b671b032c2996d5a53d1e04c765d6f573d391b36abbaf7447d734cb0d9c7b9a0b62e6905cd208b92bc1363a9e385bad02dbd2d61
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
