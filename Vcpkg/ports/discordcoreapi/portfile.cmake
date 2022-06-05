if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 432f7e39f3d7ad0e8b8adeaa92bc7978e052a9f8
	SHA512 703d9b358ce75cc294495bef88b05800f85e1863caa5e8c931fb055e5458fb25c2c35205f426a0d5859784fd731a8f2ac039d91af6000e1ca95ab66f2e1f5666
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
