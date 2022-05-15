if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 72a79a92b396cfff759cadb1bb571fe855100aa9
	SHA512 b5f8a422d3d18a1f6b1da4ed079bf96384bb76eb9ac48de55cc57f7f08ef14db423809d64313ffcb9b860946194db602fe669360c9bb1c38e10a34e1c9d9a38f
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
