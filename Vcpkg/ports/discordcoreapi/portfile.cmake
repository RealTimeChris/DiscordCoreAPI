if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 041a85d99779c3180144b049ad55797c7696ba72
	SHA512 73fe4ce4d34d818bd192f078e7dbbfed64f67ba3e66854a48b0fe1ff08a767b563db89f3b4cd278de1960032bc3ffbe68ba5566d7c5ba88c79434d380c9ceb1a
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
