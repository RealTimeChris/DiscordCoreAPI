if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 20853cf47945714c12782adb35811ff36c9e1489
	SHA512 737eb84ab68d2bfdc0b9e39960feeb821d43bfa3cedcc129fa3211fe1d276fdd783adc999695b48fbe789782879c351f6b6f8ed0fd1777b9386f68324e6f306a
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