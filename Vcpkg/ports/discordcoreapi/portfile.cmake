if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 0d0f1e3241ad48c3184011fd65b84594b127f635
	SHA512 21b869c7c50ebb38831c3c9ee1108e1c91c835a4c854d4192cb86b72eb4c76df4a30ba371b00b136204098c950e693ceb2cb057f8b7ef402c8ea39f985dc05a8
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
