if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 1ac0721e56c4168e21ce68730434b28bf24fe640
	SHA512 51a08d89f9aa8106eef7807e03d496ff2946c296c283eb82421edaff8faa1c0e907fdf1b11ad22c9d48a7af9ea0cd7ddca2c887c59d1d9d3226cee51341745b3
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
