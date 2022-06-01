if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF c9c3bd09cf8d5c6200cf01adccccc935a75c09de
	SHA512 b3517c8ad1bcb25fa38e907afbbadef45cc2b3a954a458d8fa412fe9b8b957c8d5950fd285facc8211efdc6a902271ebbf2624bc02e91f5254ca08983cd3be58
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
