if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF cd4cd6e4fe4739f63368f07a8e53238f3b9a5d38
	SHA512 174ea9c1ba80ad1e1fb17435d810aeea966972ab4beb5bdede96227fd0430fe0be3c9cfbb0a69ad801e5931bbe957739342e26f113a1f8217ada613665a4d157
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
