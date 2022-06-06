if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 0d2e87b3dc640353c092d50fe67f28c34540cdac
	SHA512 06ae1283a468c036685b16c48d53f111a435eb59e0f6c5827c7a9e19e61c472b3c467111bc33f128edeea5adf3133b5fd52d6b532b446b347c35e6ca7f58ba9a
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
