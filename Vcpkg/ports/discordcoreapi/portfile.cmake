if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 847ded4f7bbd8891fc245c58f2daff55d3086ad8
	SHA512 ba0e267ff1df85d9dc973edfcdf69cdcaeebfe79b149091b1b0ea7adc975aec88ad4d3697d3106e1449ccee59aeaac77686c9d1b6bcb17657bba927ad0198987
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