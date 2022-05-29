if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF d5fff2d33d9b3bcd911751bc881d6e625c3da8e2
	SHA512 a15ace4dbf059b2f642a2cd7ab6fa7c334fc8fe74921f0f4c7277580a7444c3473035d6f5440fbd98926e8f51ef192213048f940db287573125824bfb65efc0f
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
