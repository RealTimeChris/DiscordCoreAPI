if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF e54b20c1b9df7486962a5e8a5f9d0b3d4a1146ab
	SHA512 6693d78a1e12e07fa5bb393fffea89ea3cc88d7589f6f378aa59a57954bc604c12bd523d41c7d714b3d68df473b453c8b700077da197cfaf3718db74a35eb947
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
