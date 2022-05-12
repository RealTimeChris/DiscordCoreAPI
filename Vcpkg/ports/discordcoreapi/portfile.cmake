if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 06e7908166d5ad4c9931a6fc194799640380b2c2
	SHA512 e59dd64aa53bd0a7ad33781df75ec70fd19d3cb9f9b3a478e8e9d76ba3f11efcbf2f3a763d62732f8e66759e10085720e0d8228cd33f553a350b84c747e574e7
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
