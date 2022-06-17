if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 6e53462f9f6f9d019a77f9b3ba96f466832eb43f
	SHA512 6f7f4581654c4b6a11f680d012dcfa68fa310ef9f0f44cef5871a80c7facd57e04351c77deb25258b7469ba21cb6a27b41caeb79563bb125b082581d77464095
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