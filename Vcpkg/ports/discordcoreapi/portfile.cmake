if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF dca386427a558000f6c47dfb0f375f77e91d84f1
	SHA512 d7d2f5a23dd5d41e03965f3c607fa45209498871fcfe336ec8b9160594c679de6471f2a0f6951d3b1843759fceabfb57e405d427c731a6ab1295089e2331dcf5
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