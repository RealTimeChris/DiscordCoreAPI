if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF bc98d92693565bf1dff3208a42c62e0aa468fde4
	SHA512 6d975bde6463046934a612692d89978c74331a14fe5bcf7a15ced15b3bc16d103f152bf28c8756bd909eaccc7c85fa55c71491bacffe84e4689107f5cfde1ad3
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