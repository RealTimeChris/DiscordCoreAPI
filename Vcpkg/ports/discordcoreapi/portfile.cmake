if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 34b403088e045ee94331967763c63353cf70c958
	SHA512 d4052bd2fbcac4ab7de7752d1ef5c52fb533d52ebcc508f86fc40e16a3abb63a176118d62f1034072e6fc2a6d9bfa33573f82e0daff295f3e91cf9d82d42c30f
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
