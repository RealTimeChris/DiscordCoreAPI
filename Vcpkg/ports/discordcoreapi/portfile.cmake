if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 919fa9557d72b1a43bb2792ca2b85d057d3aa61b
	SHA512 6ded0bda2ae7e5bf27dc4004461d1e33d63cb60b024920ea42bf4e1553898745c87f7ff3f8b3eb88022b9e5431e5379ea2b761d782563d54cc919f8758d68f66
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
