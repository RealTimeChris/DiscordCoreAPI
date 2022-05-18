if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF db4dd51ae9219fdc2341c5a81f982abfe523ccb4
	SHA512 fd16270e86a79175441161ca365c4569dfdad532cb5a04d80260eb9144e0facc8122fbb657e07e53f8fc92ca43fe0f45540feef2539c283385c1b6e7555120ab
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
