if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 2def264b6a7c4fffc7d9f1883071a3ae29dc187a
	SHA512 5b1af5322d44dda715dd8d071bea1be9e2682dbccdef4b65f5b7cf0d630c05b3819c4955ad90e7c14ed27f1e3c9063bed116c13b961207a05172de4bd9a1c9cd
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
