if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF adbce419b8f7255daa3095aa6a8e249974ecdb6c
	SHA512 f6a61eb161dc58f8ca89dbcb6afb18d602e8a438c2c8855acae72e2ae5f2524b54be400a634c5a216b953e05b746c109f6f8e2cdef52224bd63b7ba23fc28bc1
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
