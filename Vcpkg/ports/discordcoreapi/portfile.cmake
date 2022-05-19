if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF b997a8c86f7c4fa3e10a144b99a0246fce8c1787
	SHA512 e5f05351f4b75ba343dbab97f19524e01c13879736f18861f376054f73b7411a52ced09232e9a9c2713fd74f2da2cecfbc8e210b4e75f93de0a0d8c6791a6961
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
