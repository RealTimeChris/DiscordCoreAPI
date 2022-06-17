if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 957268c262f4e81ab23a406f470715b51d3595b5
	SHA512 d244d33b484f7bb0ea8fdcf4442962cd2cfc305bc640b96bbadbcb82c5ff562bc73f88f42b4f4e1a3b1983ee70276f6ec2ba6e79d95e8325c34a5c14748dcc0d
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