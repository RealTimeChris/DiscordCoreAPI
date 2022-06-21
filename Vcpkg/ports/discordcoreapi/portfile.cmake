if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 25fa4132fde58dc4ba9688741c2f7434676c2cc4
	SHA512 5cfd076e02d8fa58f16f6df41fe35816d7a87898e4cc4a2c9ca484b0aa239bbe51d8bfa70cc0f8e2e0859292ca32286a3800135f243dc5cdce32dd238a0d4568
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