if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 61419546fb8c3537561f0cc6f14dfea29a962449
	SHA512 ebd979d632412784b41095907e49f111ebdac6137ff2bed3f9e68de9b2fe336c11eecc31b1e9f3b683a6f103fbe072ce9b57b6e929b30a7efeaebd45e1c6a7fd
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
