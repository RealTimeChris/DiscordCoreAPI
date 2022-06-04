if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF a399dc0daec2cc01b918a8ffd1107efc4baf0ab4
	SHA512 b291dc3be51601a5b9a23552e902e8dc1a02b445a29396e25caf890c9a92d5a10c7f1e7c6413201c1ca34f386cf79552d679fbf6b53eb92be1a37897c4a89f8e
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
