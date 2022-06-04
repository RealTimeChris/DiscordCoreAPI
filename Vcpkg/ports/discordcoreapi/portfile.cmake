if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF c3e4255fec33f0df791f47e64dd32bc55a746fdd
	SHA512 a9f34d8774cb198544450f909d0ef0bd50e34fbf26ef82536f90f4b2aa0ad0dd925cfae866751dd49ba73652823afb2c144426a54c61271fb936ae71c20da6d9
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
