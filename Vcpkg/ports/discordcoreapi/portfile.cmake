if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 9eee3fe95a92e5bc51f9e44b7d504a6b00aa0a3f
	SHA512 8b83dcd285e6a38c6dd2fce0ccdf087f9c9efe583cec7b5fe7ddfde858efddbd56c2a3e5eca63e5e2b2e105552e27fab5c18b159b0fee4a9b03b66ef06307f17
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