if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 710a12265a12402d19dc39b7ea3d106aeb75090c
	SHA512 363d78970cad6124460a17638630b34725c4ba5fefe24aeca20e816edf6c7617e8cabbf2ceebfc5a8a13bee6247021c97562b6cd77bafef526cb50ea0fad5f12
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
