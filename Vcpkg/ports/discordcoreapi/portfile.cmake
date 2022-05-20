if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF c54e40cbcc42a2d112fc4c81bbf7bab871a34af6
	SHA512 892bc01ec09d5035ee39ba1a3f6b471cbb4005879833826093942a88310f5034bf46bb87c1d73b8453b32d4e7bbb42e9e770c178c829a99f729de6cd75da6442
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
