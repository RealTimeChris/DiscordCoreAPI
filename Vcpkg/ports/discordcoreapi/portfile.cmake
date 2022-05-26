if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF d2ecd49a7a652506b66afa6f3859853bbb024cad
	SHA512 14ab7bceba49399990151c2cffc8784c1697ba5b73a676021e666e0ee71163c89cc50c63360f59eaeac2aaaf430467eb5d48020456b82f7e996df50cca6b7d43
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
