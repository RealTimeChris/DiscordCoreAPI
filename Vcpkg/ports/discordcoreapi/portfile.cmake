if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 929dc1d26c44829a341d3eb5c71e0914adcca791
	SHA512 898e59f112c34feb13058ee9794b611f71327caa1ce3c0d13b8e30108820e23ab49c64d813baa04d550d943ab2c380aa97dcf1f1a59eddf1c98aceedbeae5f88
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
