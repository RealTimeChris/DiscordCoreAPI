if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 1b6117f31e2470f96adc3d1b97f88acbe202220b
	SHA512 53769ce82e9232539a8c7f89afa74cb0608c8cf8d50dd4f8f3fa2920a1de36c0ef812a8c466bb9ae58eac3f7a5ba90cbe73dfac19584b2eef9579150e860e26d
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