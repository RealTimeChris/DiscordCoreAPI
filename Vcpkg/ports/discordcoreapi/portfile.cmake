if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 14633b48abb50f621b89183582152eeef04acf70
	SHA512 1285215c9f96f7cdb8f3d8c1f56ca6cddb874dfe85569019b8b1364e2beab81b161a748ae574a805f42fb37d681ad5ceb1239492f8790dd97b47bf00f7f3c84b
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
