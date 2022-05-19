if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 0e0347c8419a4b1c78ba43edd55d977c433753c5
	SHA512 d3a529618c37b95b19b85c4998924c09882e0c018fa0fb5bfe7cfad58468e30330ef2f5b7c486ba75dbfb5db30f7a12e4415a366a8ce3db85f2f69beb8bb332d
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
