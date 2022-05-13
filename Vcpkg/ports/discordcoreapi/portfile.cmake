if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 58e75552b79f85aa7691a371b4e0ffa4a7f23051
	SHA512 09466f4cf4b3e942591f20212c37c837020a87089f59522e9f8b6e3b73a7e806c2e2d056c09ee13eefde84763bda4fd6cf5e98624588d9fb50c7d91cd69b5ef4
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
