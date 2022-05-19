if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 9733bfdf92552dd5677b386fe74014d4e1956c59
	SHA512 bda89311f371982d7e9c32782a146c24e7e25c99dd4ef12a0216e230a4e3bdf496930367b00785e1fc7eb88b9ea89420991c3c9eedb0fb7f8ab6635b13f92f03
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
