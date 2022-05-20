if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF f0f24c467cd363c71c21ae33bb77fb551cfb279b
	SHA512 2a6ad5df505e6b4ea9848cfb94e1dc66aae9e155c8bfa10429a7c6ce9cbab1d13020104e9e668b33f73e173c2c5eb030147c5a410c94783d836f36537bf0108b
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
