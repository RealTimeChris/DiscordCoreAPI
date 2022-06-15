if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 257fdc66b3cbb7ccb99ec2912187bc2b190f6184
	SHA512 2bb6d6c596217555a4be64c30055a231d85c03f603750cc11062458dcf7426488a67ede6fc1ca8c271b5f6142ab628834a820aaaeedb1c3f850f0a4f30162246
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