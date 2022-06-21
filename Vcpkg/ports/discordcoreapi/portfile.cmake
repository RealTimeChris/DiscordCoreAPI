if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 429bf7ae0b217883fdb26223e1d7138b8856ff69
	SHA512 9b36c7f50201307850f2df1d056c09ad5e350d76d6ca7ea5a01f0ab1f5d654983ea69d84e9d61fc16d75cdf99ed5a2632d18a83e2fd67eea5c53e81d587c4393
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