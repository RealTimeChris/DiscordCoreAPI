if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 269ecdfa256969709e21e12fb5670f7d89724d19
	SHA512 ce120980e0e431bc07728a526b4b7c5e8c7df546537ae1e787205df1fd7c30e052efffda389e5f1c1a70aaf684e77bc14c0a4b8317b8bda15e12192b7d874233
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
