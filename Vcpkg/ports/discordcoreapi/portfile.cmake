if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 342a76f3de1ae206e6d8e421fa93ca8eeec0a490
	SHA512 4e58895178c40c4f8ab8cd483b050738b2174ccd50ca8039670c19f4539c3c45d015c93b5b2dafc1feeeba022269353d6a30964617ee8ca507c1393b04ba98f0
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
