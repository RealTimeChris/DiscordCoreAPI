if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 4fd4b09852477eb40b217f4c5d7aa840c1f0073f
	SHA512 fe0fcd92bc1e9dfeb82aa4795aa85fe9962af8128d2502cbc631658bf556849d8d8e1627fab44f07903a1a38880a10e2d3a21806407668a2fc4c6976e3fe98bf
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
