if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 2fbf39aad0faaef18053b350118c8eb56ad7c22b
	SHA512 3e3c2097bc58a912b89bcfbf9795680fe14fdcf181439314aec0052c7bd06ae08b5f73f9806e1481913e732185cfc7e72988c6d180e39fe3b2fe74551159402a
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
