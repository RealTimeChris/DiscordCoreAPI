if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 77eb470f22fc915b9a0fc7a596fac3e6867095b1
	SHA512 e47a7e2a82a78ca3526330ecae8420b9712f15ab75da8c6cc3cfc20c4f5c95105e8658a6a4d79d3b935bb145f2bcfe7f540f9b2cc1d268ff7ebba2fc6573b858
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
