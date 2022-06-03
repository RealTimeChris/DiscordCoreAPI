if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 5e3f0c7fa07df9d8c3faa46edf7ee390f13ff1ff
	SHA512 d6f8d443cfb4899e2bb00c0a0d313ccb1c911f108168939dc9ad05f87b1c8a1772390a2500075b62afbe3de3aae1709a0f9a836d5468f8bd1bb2b757549db620
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
