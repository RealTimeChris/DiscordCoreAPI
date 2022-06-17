if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 5448cfb278397675c1119feb86a1a60ed33e0bb1
	SHA512 87c0a1d47e71c7fd680e571ab8a41980c43bb663f1e66d2c59680455d4b62bdb233c9f1d4794ae5771fc4736c06c2d050a9b1adb22ae24ab8af66d56c052159d
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