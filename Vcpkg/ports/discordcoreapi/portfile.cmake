if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 3aeff0b774a7d69f5134d7edc34b4eb2022885db
	SHA512 23cdd35cb06def1d3f581b1a6e6621a06e490fe9a88ec6560f9e3b03c7a6336e9ce43326493c50cbe3fc5ae56d9bb95421d74f113086aed3f9b7fe3529cdaa21
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
