if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 434659aa9cdc8e4b96c201fa7278bcfc0a56a287
	SHA512 17ee7147b92f8f1403a3e3184cc69cc8616e723e562e319860e54252cebe8178432f175c7b67fdb4ee75ada9beabc7c3200ca5bbc08f7a6d512c55c1e4627a82
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