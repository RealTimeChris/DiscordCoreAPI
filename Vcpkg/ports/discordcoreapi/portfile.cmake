if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF d48eb7158f7474bcd99e26d0e0d9a32fc7056054
	SHA512 6f5b271eef1a45234d12099fdb90f4c199809fe33072d59b776a85412a336ede1833cc9b0dcb52a19c031a2816e5b97610a09835cc6a971410d3416b285d2c5c
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