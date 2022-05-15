if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF b284f399ae013329b061b77b1a52bfdd2e62a250
	SHA512 ee28ce7e217c9659e0f17c73038d96b59f6db7d16cb1863829006b270e0347eab5f9a953cd29d68a71fee21a30e98d71a26f7709ffc58fb76bd17499fa06e282
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
