if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 36148b0f84cd61aa45550d199c7e65448feb0134
	SHA512 558a7fc9dea2000a529ca0be2e8dd04a07fd8560ba3661a51006578a709b1a949752928284ec9ed7c20593e2f92737337baae2dfbd5f501b4d1f8fe6aae68aa1
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
