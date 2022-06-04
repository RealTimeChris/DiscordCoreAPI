if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF e5222e4bb297e746f8601022e6eef9e8d2fad17b
	SHA512 5acf5119cf6b1e1b9dbf791e4110ab4461444099e369329658a5c6cb054fd4e2a583a6baf7fb251161fa1a1bda0811240595754b49eeb0d18343c8e9c8e3d4b2
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
