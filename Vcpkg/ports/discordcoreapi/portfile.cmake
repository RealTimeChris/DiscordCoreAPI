if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF f086203f3180fba473addf55bb8b7593006f5062
	SHA512 c047d8ebeb1dc35f1cc28fd8a88439420e8d1b75076356c1b61193fef26dbf7c27b06f90c39dc05229379dae14db3f18bfada0228f2aaf3fb2203fea964cf33f
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