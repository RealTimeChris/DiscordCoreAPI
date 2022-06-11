if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 0e3afd7807fa7ffc304f4d6770e2df6dabba9f58
	SHA512 a8d3f9b5cc25fcaddd665476b5bf8667907eb71246786c35596d09383e55246d67e9757fed38f7f56de90d746696053a80d144bf6b761d0fc852745a125cf5ea
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
