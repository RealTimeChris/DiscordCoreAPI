if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 122b50e6858a48bc2b595dda0d4e3fb10c606ffd
	SHA512 b0634cfa16387c9251d0de7971c03010423e8a4add4f168ba04dcb702126edd3427bf6834a63935c520acaf75ad00f748ad857e8e88b6d22038e6ed26f79d3bc
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
