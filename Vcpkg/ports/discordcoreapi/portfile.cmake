if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 4f419fd0d8b0bc42560799ee8af3848d3b106f8a
	SHA512 cf92d7e891395fc38b63dc177df99cdfeb5af8b4e1305957711e234938b9f3b78628badb01fdd271492a0676515248ba0707d1a1d9d82c22e37d98c6e2537417
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
