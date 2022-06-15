if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 386be5c892988b7c28933e8de72cc1da1a289430
	SHA512 9d75444c9db52ed33e4439cd06510153d53e04a98de3c2257c3938dca2960818d77dd3477a3e3566957590cdaf81351ec3d5ca33f2c96988ae8aa7c7e6d7e2c3
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