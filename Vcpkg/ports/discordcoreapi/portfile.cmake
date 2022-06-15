if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF cb41f9e3493a3a2c5e6e8f56b57beeb149d44c84
	SHA512 1e1b4f5ad73a012c6e5c59c0e0bc43049ed542d0736b47da4de56e3062d2215522b62de5d635d279605865007b9418bd85a9e0f8c882019540f0c423c1b7b4e3
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