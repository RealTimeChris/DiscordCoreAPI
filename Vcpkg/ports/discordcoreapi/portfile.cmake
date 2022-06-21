if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 7c8f875b90cda8888f1f979af1880ee56943fdd2
	SHA512 2c54b4849ffb33f9545da202b0f3c76d260f545954569dbe0c793772ac8eeda8d66c5398cc86d3267669101dc86e72874efeb8d3927f566798cb334d5fd53c9b
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