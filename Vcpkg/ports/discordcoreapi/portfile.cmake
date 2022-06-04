if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF bcec0c6f08fc7fc769bd32913b1e47edee738d7d
	SHA512 75788bf587983118753fe9a15e5420cd90d98a6cc2c4256ceff8cb281408d85ce1607781eae8877ae3d52fb844f9e18c8f97e61cf2de91f672dd05677d434ff0
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
