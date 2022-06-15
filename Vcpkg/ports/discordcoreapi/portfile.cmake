if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 36c6339e50aeecfd1422a119d0e2f085020b3237
	SHA512 5e3b0ab48115c15e8151a69e63776c79aaa01b1ba438b541458b90a83bca2f61e421a06497c8a7461508a89806ffb01fad4e7b84b7d0a1566e6f3d3feddb9d96
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