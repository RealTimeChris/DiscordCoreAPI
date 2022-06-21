if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 213cd844d79b53c452ac3d756d28c97729364ab6
	SHA512 341b96ce0f48d8d3dd50c1e981e73ba5bb71eb91c95fb7c93c2c88446544bcc87bbec789ab19edfa4ef6435e24c6f5c1cef5b78fa50c39e4748564adbb8f62bf
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