if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 14110010b7c917f39f0cf083b28b1c8ce5a20e67
	SHA512 0afe3d9a169232978dc0420dbe66d785ecd9a42d2864279ed704db5973099a8d2b054b6b17b0e0900ab9f1938ad02e1e6910237fc6e128669fb136d641b85881
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
