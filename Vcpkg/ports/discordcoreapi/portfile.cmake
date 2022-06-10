if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 62affc9b9c961be9fb8f8499772e3b03b142f65e
	SHA512 12fe2994f14a78497a046dfec9ce089a6581ccd67c627b4fc5a211850458a2a6f5c382e77e7ad6a2944b7350445f181d16b9016e24d9738d7aa2af3cdf1561e1
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
