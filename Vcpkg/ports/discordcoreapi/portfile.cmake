if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 7aadab7b4b2fa04b64de34e3c882d42d7e835bb1
	SHA512 3e0cb607c1d091883a67fc9c323c6c688a57f09c0885b85ebc1529440e15445bff1b58b12307c4101d911c919b7b431a642060b7df91b2bf58c3cf6124691f4b
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
