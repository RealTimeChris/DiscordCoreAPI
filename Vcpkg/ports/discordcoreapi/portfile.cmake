if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF bdae2d9913a820da5c7fa76a5f9f7f6945c92b27
	SHA512 d6efab7f9da70663376bce668d046dc91151b1cbb83bb0421893258c27bab7476dbad2f082c6f450dc68ecd6c9a826152c8898d6cc0201741f4337bb2df14c79
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
