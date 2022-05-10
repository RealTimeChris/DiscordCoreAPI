if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 8651fda9e7ca04274d42e55b0e3e8fc6eaab97d4
	SHA512 7f3d5f64b322b0b720e8a24c3f2ec95a0a909999bd23e2c200103eac74dc0cc6653161108d23fb3d6914802cc5c288edaba620655271ac44287c0bb3e8230ff8
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
