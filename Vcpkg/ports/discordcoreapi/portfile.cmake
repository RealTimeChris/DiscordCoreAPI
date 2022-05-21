if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 91184783ba04f79b2b0261e5fbde7a25173ed14f
	SHA512 9ed6c829fc4418ebd70998924e5acc6bfc16d2243214170ffa8880c12829a314c25dd4ad4caaa6d15da6858555247be30536fe059fed4b6e9c686e5a95296fad
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
