if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 7d1797813f57be9e921347a5c49f4866d6be1df9
	SHA512 dd5ee5f17334b0f79601ed20faacf1448cd4ab6584378bfb3c20e1f3fe522a00ec04e629e4dacf8b3b004b70a39c9e526c44971ad3a46a69085409719f7b2dea
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
