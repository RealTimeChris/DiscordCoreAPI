if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF ea158c97f778fb60d4eac03d185626c3691bf336
	SHA512 0710fed5be29b3a7a442df4d35aa7e8b6380d6838c2a4121c44c9a07f8c181c733bdf8a7aebada0b97136476cc3023b152dd120aebf7bbffdea9d1e67fd966c0
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