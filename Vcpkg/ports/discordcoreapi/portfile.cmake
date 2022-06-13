if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 3cefafc827df7cf4d86eb304f17046a71720356e
	SHA512 95dd8c6a1939bb37d03c79cfd3b8ccb0038f5e2a38083c3b0bf31698a64b060f9eaee9ac68b212b752d029ae85dea59fcaa785cc73e7b1eeede2d7fa55658b58
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
