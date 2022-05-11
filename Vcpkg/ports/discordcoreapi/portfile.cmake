if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 6fdc8a89ab30a5e54f40f327be996248a85b24a6
	SHA512 b3a53fcecd0b07ce5fbe9421375bb2625e956b06baef6b560c1d1308fdc14c32a78a4a79e3206d3c3f4e2f4d5daea6371688db84f258297a0b986bc6d418e1ee
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
