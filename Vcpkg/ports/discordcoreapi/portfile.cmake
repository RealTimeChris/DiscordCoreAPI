if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 64f320a1a3e62c7ada22d5f42337bc0ac9fe61cb
	SHA512 d667940bde0df38e8e51a5f41be52519dcb40e6417d68c98b2dd9df82bcd2ea331beeb5c3d42538ce2762ffe035804a4e02b8536268bcf52e03004fde2f08f2b
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
