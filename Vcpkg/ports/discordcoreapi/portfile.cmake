if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF d08b05272a1457bf4f086aa8edeb6ef8b4f39fb1
	SHA512 8492ae38b75b5244f82406dcd7c55c82d2095142922d072437717f96995f2731edc869d86079f96b7da8088f9615ce34ee0dd6593fcb47ec160bdba042ce17ab
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
