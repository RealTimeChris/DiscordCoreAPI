if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF b1f521cc0bac8f965c525675ff9129a7c3638121
	SHA512 bf2e7f6cd42f3ac44de536b3a31b0da94ab23cbb66a7bab533f57fb8358389a6b0fb79ae8560915a4ff6595b495f2e471fcfc8978ccc522c05186c50970bd606
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
