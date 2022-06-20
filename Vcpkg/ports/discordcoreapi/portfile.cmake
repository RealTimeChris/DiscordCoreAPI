if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 4805570d30a0782255fb6792a21002d60d344256
	SHA512 a669ebdfcc6df27e137c3e0b7fb37adbd396c5c6c1ba3781c7300f19ec2d1b963f49b2c8504bdbf2bf71c33892f8000f371a6ef69d675329f74343e01e869e42
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