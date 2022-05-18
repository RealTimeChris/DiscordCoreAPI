if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 8c181d039b9df4c5b308ceff822886f1f40850f1
	SHA512 3e38394d6a7043faa3a1a06b21dd8a91e38fd9b468a85e070247205082d5b9399d4fc197ab1bb0ff29ef9fafbe4a4b558fd3815933d518f7582fcf3db4b2621a
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
