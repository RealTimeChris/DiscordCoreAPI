if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 98c6107726b47d3caa2372a14613878964906061
	SHA512 878690a389916dd1f73f2fccc3ef8de0706b52235aac7db8f8ade97e5de0aad64e73b694305fcde8623d7feb2e05e3f86463a3fa8c8dfd9b3af5324db9aef50c
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
