if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 8bc55c266cacca0c7d82fbf15c4e021bcc73ef62
	SHA512 307b437fb5d2dc8cd832ee82bcdd36c1f1ee720bd5e56f1ecf7f68e204cdd547d6ba047abcf43f816b8955914dccbe09b8fbf38bc3d4b6f6797d64eba6bc1442
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
