if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 2fe45bbe5e1c0468431d52311ba4fd562938253a
	SHA512 70c135573529044aa0fd21a97707da49220edbde8035bb903bbfa661ccfbd012f2d176af0e75ee4a263d3e1be88fb677cc13afb45b304cfc2a52ad6a7e2a9467
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
