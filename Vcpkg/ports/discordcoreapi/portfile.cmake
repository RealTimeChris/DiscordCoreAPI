if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 28aa62c6f01da2f1fa7a93d206c6b71c732382fc
	SHA512 16a9eab02003f637e387508f092322802ec7b50a2751699c443ba87ceb5f0a5e16eff20508f4ee60091a26acd04ef048d231bea94fc2669f613cf58a8ddf1745
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
