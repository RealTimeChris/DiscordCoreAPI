if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 8b586674a8621cf75b1b054cc7b95ba15578cc7e
	SHA512 9518983fea8d3fc88576f3bd3d4f01ff29b0e4dd01e97d44a3f3784546e639ac38166db545f22544fe59dac1008d4ed379344c8547a487d35e3faae7b740f8f1
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
