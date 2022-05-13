if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 37c57e127442ff0c3dcdef07ca3b9644678d3eae
	SHA512 d28a9be17ca2838c75468a34761f0cd01d66e3a7745089f27e3a537cba59303d54a46de6e8d317f686f23e6f0379431ef0a12b432a7a16991534e84be97ffb99
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
