if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF e030edcc70ace659a83214f1f69f09616ca1cc12
	SHA512 a2c11c28e3b7203a362066a2d4850e2c34c3255b637edfdc4239be4d32ff3f3dde9f58831d5ef334196c6f91a24be463f080120d9068116bc549194dc6fb0ccb
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
