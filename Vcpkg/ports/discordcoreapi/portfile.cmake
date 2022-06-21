if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 66ab0f62ab90d62791b20df09efa8cb0aa2fc44a
	SHA512 3703542049bd7cea38669b49b1f815cd87b18d52cb758c761e981a47c286f102a4f8fe31f79a0debe5d02a39689df8376bf9c443f2b1f33c1476f4407592c4c4
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