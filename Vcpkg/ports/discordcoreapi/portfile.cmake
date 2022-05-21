if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 8d390f8385fffd1559a314324bf559379f36fe63
	SHA512 ad6bd98ffe7b78280931036c586d2461d33b58fc5e32521e784943407f9d91220340f9729c9e4337e5875dddbdd6e7dacccf11ea9ec55091bd0d23a3603f9f77
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
