if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 1c2cd0fc36d1f373af4ee21ff14d35da84480983
	SHA512 375b9d0979294ad0e62c69a9d15480366159bb2658457200d2c029ad8ca257dc29ce92ac0cb3f5effa226a8aef21f2102f44ab7eac3839c959231b57b0630abf
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
