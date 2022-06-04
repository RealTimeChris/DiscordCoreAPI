if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 7bc4f4b1c47e93a93ae01ddcd3156b638922b8e9
	SHA512 d19cd6d3da6f8dee00721e992bed288df237f44f64021ce2262349ef5f06907e761db31c07b7a124c132047d627e5f3b4f104500d0949caff0f3881bcffe6072
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
