if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 54da5fe94498a1c1643a03ccb93ba4415520f36b
	SHA512 06b6b638bfb792e84b434d5d19d8af5bdb4affb008d61d0ac924c1bfcfda00229926df5145beb1e165c76ad440e80554dfcb5ad5fb8d3e4648df041c25675350
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
