if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 067cb8b710c4f7ff6d70e03b53301f1ff90b71d9
	SHA512 533b6cb3fed55f68cec85252d75942483acc12e534a0f2c012176cb59cdd81691a2c08a606b2fc62ce3e0512eaf180efd6b083d19bdf4a6c8bf99cea5aae159a
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
