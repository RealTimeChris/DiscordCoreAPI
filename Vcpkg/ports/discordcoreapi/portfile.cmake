if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF b5739ee6c5e0c77934a178c9f6de66b3fe268fa6
	SHA512 ed1b18fb6f07f9ee41be321a966319daf210611b6ed058b57cb6848aa59f4a093b7e61f5e77cad7e109f956bd7bc8d2186c3cca6ade5c45ce9e929a9a8634938
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
