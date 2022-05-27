if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 2b91d076e7cab35490eef9c8367b25c7b0917db3
	SHA512 8a23ca3e86a30829170b85f9b014266f0215ac1940a5d872b92f38ecd633627e84331b880f73ae5063f1a9dae81cb7702ff2b0afbc12d9758116ae019e9aa88f
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
