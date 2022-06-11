if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 1e2b3bf5cf2b7c4263819abc535d6778e11cf03f
	SHA512 d7bf57f5ad77c9cf206f5c3df1f97c6bf8ea24463e2f01993798136ce3bf5e2976bbfb3fca155d7a79595213d04b877a2f8e723f5b34a83c13a0923805590f66
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
