if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF d9879d7dec87c0c24c83ad6740458e3e85763833
	SHA512 01f590b045bcb3ef3184c5a71ac5efb1a5644850a577f0244967f43ac91fe88379acdc207c6743ac01bcfd900fb3607bb0a48ee8ef206f6dc161104fb15b1695
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