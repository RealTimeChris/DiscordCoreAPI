if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF edafe9e5d73a52f8167d8dd65f3a57d3289655ff
	SHA512 e96f0700fc06e05d4ab6d64f099fbac6ff55e1904e34562b76c84260b6d2d2daed08f4c1650a0a18b3facfd9bd5f4f0988ab79230ded20ddd347378b0b8aacc5
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
