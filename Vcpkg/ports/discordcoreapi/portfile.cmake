if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 9deede39597715822a379aeb97f875eba766f1a3
	SHA512 f5a63b4685e2033886b8ecd8b2536dc50bd19be7c0ed5208747beedbf8fdf38f8ca8ef6e5c094050ff9c0d9cf5c35167a46607c8278848827a4eb7d613c08b62
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
