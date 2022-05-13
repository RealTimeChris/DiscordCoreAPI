if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF f56bdf54d9e3ac33b22d365701059b4ff9a1db8c
	SHA512 f0249410a68733c3c7b999024cf335722a6a2eec831dabc86079481f3abe2ded0ae9c953b386acb86dc9b9a8d17e009fd79ab1fa774ab4a8f1972553e4009af1
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
