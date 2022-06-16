if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF a0bed8690ece734af89b5acbe0f78ee438cc337d
	SHA512 b54fbf05ae6ff8a133b9f394168a00e10cc1826ea3cfafce4a7088831afc555e5af27d17a8f38a214a958545c1b95cdb3ab7f0a0c8a8f7b24d0388f16f309ed9
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