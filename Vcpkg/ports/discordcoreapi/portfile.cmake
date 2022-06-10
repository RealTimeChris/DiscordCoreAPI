if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF cba87faec69f464e78e6545bf5b732de72176737
	SHA512 6b0f4a2c36f9869d9837870bd406aa289481cb07063084aa4db13ee87890757e3d04d138e1be4605db72c0c7cbdf48e81a1d85fdf309c3c89a7aaf18a3debcff
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
