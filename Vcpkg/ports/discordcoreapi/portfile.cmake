if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 7cce9dde2deb5340fbe3218b10d8cf4b36f85237
	SHA512 c4080962e56795c547e25283f44c6e675ab42ee3ff38052fc39ffde88f32f273dc35aa2ef7cae8cabfab96d7ebaafeb40b5d82ae34452b4e91d8324f3df26581
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