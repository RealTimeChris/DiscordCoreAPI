if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 59cd83e7b9aea14dbb773852a7b5575aac8111d0
	SHA512 75eaa6ef2375c9050ea07f4bb4f21802bf770b214abf8d5527f00f634e3ee2e0103b42e7e103880245eca8b82e1b0681097f0db2ccf7480eba2884b037f47990
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
