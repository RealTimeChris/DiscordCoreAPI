if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 6dc68945a15e5744d84ce2654e1dd79b1fbd1d61
	SHA512 37d79217dc35bd7ee9e5e96661ab7d82e8cf2f46c59442ba3de80ad1b129fa778d9dbe3af06ddf1fc9532afe14881ebb4e542164be58a9bf44b38c00c05654ab
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
