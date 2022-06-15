if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF d49828a9e341a17e62268f9b3901554962f8c976
	SHA512 42a8e6d550db67fec9cb5f3a1398b3931f6f5d36e881e11051d7fc2a8dc87fa38e77cb3bef45577e7d8183907e5e87002ce6f020f57b337972c588c45a7fe3ba
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