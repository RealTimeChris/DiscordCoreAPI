if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 79d28342b868ee450fbc09051acd4114a7540b7e
	SHA512 c524ce255009b18bdbbd83f9a10da8c4a385d97850d609caf66d5bbcd9908bc66e8dd7e673fe1f97d141e8f61e30e2f35c0cb581eb89440a7569e54501aa0c7e
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
