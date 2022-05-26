if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF b01a8dd03fe5761c4be3e73c0d4d4564945d7820
	SHA512 62707a4dbbf0e200c8a3321c34322115f632893c9bed7bb2b664b723e8e7d26320c5a62f821ec43a99e1e170b8a88d739a4b6cb516c3d02ee713a1fbe5a83d05
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
