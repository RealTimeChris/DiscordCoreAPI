if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF ff1c126e462abd5ec81afda46d6acafe59bb0ce5
	SHA512 8fba2b61f7bfe7f96217f0af375bfd302f1016326fa53c1fa5a36c75f30ae1f051ec7283ee8aaa82d3997a3e99df2d1f09e2609efe477f998b4d99889f79091d
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
