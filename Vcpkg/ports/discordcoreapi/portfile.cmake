if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 711341570abb590b855ea124dc96fc736f68e767
	SHA512 ef39ee25662cc0c6cb9dc3c617a4680ffcf2e3cc9bb315eb77da6aab2d97be2b353704026d9ad6b688a1d3400138fac2f9075cb95861a2f1a1e53f2faf4beb54
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
