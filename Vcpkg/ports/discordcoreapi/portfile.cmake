if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 3a9d41dbf28009c5f97f2612d92ca8d3e5d1f1f5
	SHA512 3d39f23c079d524209c7ee708a12e6cafa2296eac4c91904fe54386d507efca797c9857cc524eb075117860f5f9c15bd818e1f0557be29f8cf9854de3abb13cd
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
