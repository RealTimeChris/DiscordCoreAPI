if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 46ecd9a3544af6d54a3fbdf6a07a5f0f6ef412fa
	SHA512 979d754af0f37700452f684e8296fb58e51ace55b0df3b8787540a3955e8904db3a0b528f824b6bf501829b9b4d965c27353467fccefb001aebcc5f5f63b412f
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
