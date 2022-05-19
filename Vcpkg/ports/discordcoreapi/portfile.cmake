if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF c0a8ddbb93e15e55e2f57f878327a707cd0a94af
	SHA512 193ed3a18ff2211c57f8e652582afee8894b729cdefaec5fd0a503d2f8f3a95e6b64af8d54ef45ec77417ff26ab31bdb95b2ea80a36b62c99e65854733edce9c
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
