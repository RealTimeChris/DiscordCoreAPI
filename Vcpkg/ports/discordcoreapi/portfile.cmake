if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF f2807d1fbb10622abe32c050d38152712ba5ac1e
	SHA512 32e5fb4a2071cf503975b4195f9fbdee1f3993cb9fe34b7d6b4ab10ba76ad1d3eafeafd04488dd2fff6ea28e22cc76306c15994da36f6f8594f770871e3f8184
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
