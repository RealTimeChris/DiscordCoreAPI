if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 80d2778d60bf42150311ff133940da20233b1f54
	SHA512 3b75bde5ca14d99b51ec4285c6021bdbb911ebb4c03947b804c73a28056d88e21c17e291992a156cc9fb748bf6804aabffd14d9f7fa10a5b305b7bcf0db68f98
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
