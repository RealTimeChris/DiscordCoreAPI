if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF cc88174fe4e1eea18875ecdad4d8123f5aed16c0
	SHA512 081840bd242efa4c935162ee3bf573fbb1fd6ebfafee714baaa78a7bf422d21190b5fbaa82855a6901c077a6c9a5ef2301cb38be79215d8c07c777255f7e1a4d
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
