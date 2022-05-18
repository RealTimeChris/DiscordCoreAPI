if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF da18c0e4da2f5c5b5ae473b81c5872ac6f80eefd
	SHA512 63a8ea24a5e6904bf6d364544d7fa86b161557e0a636c4e6c3052024660e98d0e01c9ce1b102f169e7bc0a5ae67a62af3a160e7def2bb1d4db19fb1355a8477b
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
