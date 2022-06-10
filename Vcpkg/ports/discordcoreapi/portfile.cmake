if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF d30c5110475d3ab16aedd3bb02a3bf831dcba9d6
	SHA512 e1994680ba71e275a9c88b46816a33a736843661815abbba650d00b7ec1e8f1dc575a049f7d1af792b2d47900e6959cea11fae34694a2716f8058b83a9a91c16
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
