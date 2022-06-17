if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF f4b0c79af4c3d4a616fb5ea279df8ed1f6ccab3b
	SHA512 f747ce6ae76c78e1f032946be4a9a5f6df1b673c9c5519532455313f5eb402d65f6f67c713892b62e1b8fa402e9670d5c65156cda6f21af1da939fec72b5fde8
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