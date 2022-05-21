if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF df21c62ef666e2ee87be24b4e069dfa60b05dc9b
	SHA512 023f71b4a8c730e6dcad1b573bc68eb5f1cf8f981c35e10470d22df6627d46716a41c6920493a1fd9ae85134360290b3625db046233ee029d3d0252d97d49f1d
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
