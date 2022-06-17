if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 52953e91189b1015ae77593d5eabc4beaeaddb62
	SHA512 cb4152f9eb9113606f3b04e1a9bca360629adc2cdbe4aa191b1f4d7b1de23f7d7449d2246165b8e148eb1bfabb9ec6646eb6f2c96d73d59e7a4fd265e439aa61
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