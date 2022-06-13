if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 70a7577cea1798fb62629b17925ef2f8a51bc5cf
	SHA512 ba9baf9d5efb7c3c8fb6adbea51051e565d5633ed16c6e0b5f9e743275af1db70dae7900a2817836c9166c41ea2a2226f407faf37c4f137c802d4e390cf2e9fa
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
