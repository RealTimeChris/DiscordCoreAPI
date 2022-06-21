if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF e0772d09ef0bd7218e055a8aec3666980768c84e
	SHA512 f44f7db2ff6f35094d712415fcaf3ce69c7d300c4679a6f83ce187639a9726700babdb1257d79c299c1e48d04e966653fc49d238aeb47ed3f91eb3be05e7cc4d
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