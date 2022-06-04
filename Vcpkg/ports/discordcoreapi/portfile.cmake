if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 78ccbbb1aeccc98005a5f5256d91e5b497220d08
	SHA512 64cce7fe37fe5b723df990a68627266625166fdd04fe44959dd8b2ec172531fcb3c3374c64579dbb3ce4a293cb807a34d491088af39a7c12fd63b52d2dbb0f2c
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
