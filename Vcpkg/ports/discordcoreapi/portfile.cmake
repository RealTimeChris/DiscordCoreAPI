if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF be884b6ba6337539bb1b84f9e9abce204300e3cb
	SHA512 7f77d252872e025f0a5e008609c7f0eec334e289859fdf32235c7af9b18e789008cdd3dc10bc394c8aed9d9151a1e37b255921417ab4bfb37248d97177a96ad5
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