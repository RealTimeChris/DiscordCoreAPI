if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 720aba53adaff1ed47e763752cd8baaaa16e8f1a
	SHA512 992cec4d5eb5cc2e07b2d39a57b17874cb796c09c5b0f5439becb0faef9caec808f2c66e0dadea94d55013c243579d9429b67d4c0fd7eb93e7acb1a0492fc5d2
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
