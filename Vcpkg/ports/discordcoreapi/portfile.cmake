if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 58e75552b79f85aa7691a371b4e0ffa4a7f23051
	SHA512 0cdf15378f8d5c27d6187d154cf6f0b038bd956544001d1d35f6be79cfe3da03c4dc1c4355d5070e871c42f8cbc7134012ebfcac30035874edccf3f75b583f54
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
