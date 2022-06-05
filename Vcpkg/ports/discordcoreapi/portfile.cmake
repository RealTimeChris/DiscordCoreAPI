if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF d8a999f55dd66da8df80da28db068ae0d80f36a2
	SHA512 e47fc90239891cdd7ebc442f54a233f765ef4c70fa8cef52fe94f998861c487e9c3ade0652f12ad0d5e86fd5619e9c3cbe5acbffaa77d70d4c3d227e49c25629
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
