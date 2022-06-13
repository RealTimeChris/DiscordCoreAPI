if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 4e3efd2d9a09e05a9511328c4398036a66b48961
	SHA512 de87cd3a6be125e6c8526cd311aa4113b9be781437ee6b9e4da04f6f8a1be491e5427ac84a144e932bb71c13274e87d3c6ed0eb17b0e6c6b80933489ca2c0336
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
