if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 0a28828e9914afda8a2f6d00d44b7433e0b7b618
	SHA512 ae4b5aacb95fc2bcebcb2cea75bbde2ebefe867f339224e1a9e298c4998c08cc77fe3324ef9a77cbb3a6e6ba1c708472cda897aba02368bcd14bae4b94b66dfc
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