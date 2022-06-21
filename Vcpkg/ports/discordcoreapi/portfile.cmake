if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF d14ab15556ae428f5c779304fbba5bead96d5fb7
	SHA512 d44d932feea207c5394508d9b4313c9e5c93c55dee0e320f9583e9399d9e09cacbbab42689ac7478b4eb0b6f1a4f36e8af94bb3ca9473ab8ceb9ddfbc0dcb9a8
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