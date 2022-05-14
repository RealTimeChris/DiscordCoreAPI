if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF ce600b2216ef328d69b9d4072fa7cc96cc6177ec
	SHA512 71bfd567972c8223c258e175e01e270a329f1635670edad6b47c9af2ea895af4474e0f8fcce0cc0158b81a4894dc385a926768d68a8c28a6e29ab286af1f55c0
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
