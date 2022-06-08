if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 6d7cdafbd272f8f509d3473da930a5d89a69b564
	SHA512 5cc7dfb9d28a51e5acb7bfad117761d94e2ec098cb533e4da81f486a702fccd2571ad93e8f91e76f3429d4b4810a2174cc17b09a3ef13840af27ef5dfff1d332
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
