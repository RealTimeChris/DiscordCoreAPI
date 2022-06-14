if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 456fe9ef6fe1bd04f514978eb6585006499b191e
	SHA512 137e3c21678681e33e15ed8c16edb7c707eabce9cb200a4ecb4b1b6b51973189e15c049c43e99f5d28d38b29ef36bbcc4c2d057b34c3b167ed798d3b5fa691d5
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
