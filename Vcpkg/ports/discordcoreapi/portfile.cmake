if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 8090f219f0a4b1ec3fcf9c6a62540fbc99002b11
	SHA512 1b7944cbad99c645aaa4d588af8729f45e820f0ed806ddc1c87e120b375b7bb90504c59e740166e235383a8271c8ec28dd0e6229a289a5e25f1fe759106b788a
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
