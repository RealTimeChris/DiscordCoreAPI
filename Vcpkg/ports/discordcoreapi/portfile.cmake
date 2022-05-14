if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 46b278959d2d1618ee019203eb49d45f46adc374
	SHA512 a8a004390def4cfd6c3759a318bc5afe26f8d71a5561ebf46eaffc570019f2c345799b9ff51b6d33ad446e1a14ff1d9d6e43c7ba8a9f4a27cbdafb3647da4ac2
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
