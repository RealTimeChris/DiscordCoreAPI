if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF afabacaa331106f5150d3d97538719b5e0169eab
	SHA512 ac2e6236f5d4e8f211b15659ef2f0562ecb0529b67054956e9d676992b988514ff1215c72f90240a9a98653ce069d187fc9ca4a9ec9368d5d5ce407c67d4c482
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
