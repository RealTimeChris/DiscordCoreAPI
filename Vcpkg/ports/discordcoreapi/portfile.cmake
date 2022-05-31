if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF c1ee5380c1ea0c68b6c8b301761f9b0d407723f5
	SHA512 80370d535e1ebd3acd9da446f78cf83e1efe3cb341c52eb30b5c471a058df9af78eb67080b5fa4dc0e148d722472a224b8af748531ba44c7a62595afb27bbf29
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
