if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF ac3f01db7a52daabeb745bb8756a2cb5c803565a
	SHA512 a656e5a2830a779f5744e24ff5ab4239422d8748eb1918c26775223cdd888baa744f5504e1b898fe10648269fc1df04ea2b2be6ef1ec620b9f3b97fe2a1789d2
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
