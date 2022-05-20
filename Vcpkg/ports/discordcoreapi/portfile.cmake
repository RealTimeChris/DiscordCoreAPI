if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 0d8b781ed3f0dfad5a6b3d60e0329905675bc9b3
	SHA512 b59733b7591ef25753d26721a607a07831bd5c96df4df6d4f4aada4f9cbf9ab7879c9c4ce71ce882312fca71d5cd7ea45375e332874739a2f448646e5a014da1
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
