if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 95b5e167e897a507b9132729928479f1c50a67f9
	SHA512 718b9dac74fdbabf60d57e8d0ff00dd6bec95455e78cd9247b729b0e5146e97e202909578cb8a0f54a6174069be7004e35fa71bc406adb94d7e4aa5de6c1eb81
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
