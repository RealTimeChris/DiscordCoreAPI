if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 5891b50864c4c6aa91d3ce78585dea228c1d8b8f
	SHA512 f0d88fec04bfc76515a03a8052f2ea2e2105ec952b3a1a31c5e3b24c2bcc9ca4ee2e0381ad0518c29605e1730fbb091e24706e7e22c3d34eb8185dec70a32170
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