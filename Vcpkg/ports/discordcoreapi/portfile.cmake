if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF ad71481879e9d8f60bce22860f25fd30b0644f56
	SHA512 dac1bf093d1f9e3318f9a2d4f80c657382e37d0cdebbd8770f46b82255e4db44997a07b33c2b67cc81c62f9616533ca30be43c8bcb3b24fd61513e1ca4d944c1
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
