if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 79f2f1bd0acd84de6ee4a8711dc1797aedeed1cf
	SHA512 96d628e55f26889192e477df449c8751d2e408d217178f982c0af61b712fd708aa257334eeb767363bbe88205d3cd41e00def57fa5dc7e787ac8a29cfab42beb
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
