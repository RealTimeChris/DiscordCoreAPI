if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF ca80f142450fd59fe7f3d295e1410f7534844322
	SHA512 7cdfa1f608113f7329a807ace8e4c59ed76f3e1258683be19b9f3e9eef3c6990e738a5fd289e6e3d5011e938a23a74aab36434b492660fb8af4d93b76fcf6fef
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
