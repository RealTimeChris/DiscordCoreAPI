if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 029c00727370e052c8b3cf5d33e62edde9a0a2c1
	SHA512 d18c660b46871d399a9899296b55d34a9c9279aa1e8b41f028cbe128c43e588c226bad06cf468e036b4511162dd41fdc073425ea5d00aae02ff6caa2ebe5880d
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
