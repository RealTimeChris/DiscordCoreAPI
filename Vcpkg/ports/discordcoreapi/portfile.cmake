if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 0fb2a9d51f937e40d7580c83679213e92efa3ab7
	SHA512 2866ab5cec4bdd3198eec9efac616a7c962ce719fc9f66304798b74d9f088da233b565df3a7856ae9b48c740b99d4a9b8c37f1fe03a7189df746d1dd8b5e9410
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
