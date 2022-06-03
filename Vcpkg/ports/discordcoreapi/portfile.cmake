if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF cb41d53e47c255c058c963255fa57014d04771af
	SHA512 2c6fc56b64b95c3772e83703ad8a2eace71b7ffd1fe42e06405703d4ffcd84f555aa5fe107a117b86eda417a80715ac4fdab6cb16eba1999fb1b6b4eab72315c
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
