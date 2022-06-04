if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF f20c51f546e5b6be79a71012b3081ed888840386
	SHA512 2f2183775a2a6b0fe7a8fc88bf98fb1e9e1f7d758f5bcd70f3be3dc88e0d0b51dce3cda666f6f50b6329bab38e663c6c294276abe3c667c561169e92d9437fc9
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
