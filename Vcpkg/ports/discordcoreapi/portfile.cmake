if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 91effdf5b73c59ac24ea1fa789d2b8ebe83c61bf
	SHA512 4bf10cc26cbcfb0e53ebd0671ca02bf8fb9009b1cea3a7760f4e4223c2eb6efc3c6271a93853642842471fab49945f034a333886a10bb550f168677db9a14fd0
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
