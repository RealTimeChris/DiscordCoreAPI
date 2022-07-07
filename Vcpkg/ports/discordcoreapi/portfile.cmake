if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 51cb6c063942678846ad6a0c1e3e5bf2de627cbf
	SHA512 196a1c8c38fe2791f44aed3fb58eeaf2d490845b7fcc9d0e6b01cda259bdbbe98ed3d3746fabe2c4ecde7ff6ddf7ca6d013d09fe3e4d202c1a1d57971373145c
	HEAD_REF main
)

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
)

vcpkg_cmake_install()

vcpkg_cmake_config_fixup(NO_PREFIX_CORRECTION)

file(
	INSTALL "${SOURCE_PATH}/License.md"
	DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}"
	RENAME copyright
)
