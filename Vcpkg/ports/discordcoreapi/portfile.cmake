if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 1d1aca50cd86043e1958cac31bbf1f26b1bea352
	SHA512 56d3f14300dc9c4b1d80290c8dfbf136f3a8b71bd206efa77994707ec03aed67fa229089d48edbd3051e6754e8aa41ae3cf5a6cd7f09e1c5a981fae74c080e56
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
