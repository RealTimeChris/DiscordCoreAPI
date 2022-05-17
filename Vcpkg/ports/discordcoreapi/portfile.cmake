if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 54953a0c415df8f22bdfc714536b5d75032151cc
	SHA512 8351a5ddb05f439432ced028cc7ee2aabb9dc539f28dfb4892aba8319826a1737cdd5686a4e51a6b24e6f782913b0b7fffe7cb527b54684da2b3454e9613765c
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
