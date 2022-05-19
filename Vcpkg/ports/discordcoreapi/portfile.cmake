if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF d25e12aede5a370a16ba4171b376d4f4968c2c5c
	SHA512 4326e05d4129b6d0e0dd37ad9fff29edcf355c0864e070dc25f4cb549eee11ce71fb5dd31d5cc20ffa73d1105ef82eccafb3230ce800907f0c86c59759bf3192
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
