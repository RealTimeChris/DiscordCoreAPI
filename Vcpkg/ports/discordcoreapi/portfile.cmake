if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 323daaf95fe077b6286f21ba6599ad9308e67fe1
	SHA512 6646bdf456f89ccea8010aa699ab9c7c19bdb283c0aed7dde5001743a6c046329556f2c025adf010d86b586cb9aa0db146d7327291c21f2ed5fd5dd69885dcdc
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