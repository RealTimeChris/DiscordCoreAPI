if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF ccea9b9957398fecae57817b3ac47b636802b5b0
	SHA512 c499f9f9b98bea64331af3a74d62b7e824b78cad13bd40e30257bd8638b68788009c2a38aa802eedfb9bd6f5b9f8410d6ee08bee27d668c70d8a873bbc2f8450
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