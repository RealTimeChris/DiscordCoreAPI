if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 3e3a2e773613b9eab255aea4a5e9d60c0e0175fb
	SHA512 8588259d46fbc73b373bedba97a3f708786b8b87833d61442afbf49071320bfb6f1c0663277d7e84fb8c0530d370ac375cd667c85f76ae7cb19e3d462a9b2b7d
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
