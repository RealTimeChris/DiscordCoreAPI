if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 2381a6b338e7ee4106f4b43d5708fad52e8de68f
	SHA512 238c4743c2fc429a6c27d71ec9399becc0b26e142ca8b474d29a94b06a3522e98688e35c2e683f8abd06b53c2edb035b906fcb6b0f61fc2762b4287a8a95f3b2
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
