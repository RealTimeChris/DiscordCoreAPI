if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 44aa2d93f08ab4991fa87ec8c0a7d4b598fb7fec
	SHA512 14568407432d09101281cb0c55fbb5564e921e4fd1ea4ea486a7c5b583616c1cf27fbb9d4460fbac9d14b7526e81940fdbd5b3d471f9b5864ea95ca8581fd610
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
