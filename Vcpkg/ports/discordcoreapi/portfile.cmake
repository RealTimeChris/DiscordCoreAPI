if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 5506d111ab009adf62ce262be1d2b06f55736d56
	SHA512 2c5b570cf77ce8afe41fee70257bf9eeaaebb976d8033025bb4faa72a32da16b5506e191e6d503557dfadaa37ba8477d5ff4f99597ca6d50128c2d767fdc9333
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