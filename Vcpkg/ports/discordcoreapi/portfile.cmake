if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 57297ad78ecf05ae0d041d42b362d6bd134a874d
	SHA512 971e54c567f85dfecf2c8d403c232e399e96e62160fb936cdcebb274af7b9adb60bc65f573b7cbf3b34e603e8686c54de5071a3d34d511335d5e06ec8eee40ee
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
