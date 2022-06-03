if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 38f78ee8fc141ac20ad5df3427edeef3410660fb
	SHA512 d33c92059763bed93f521838fbd50004c91e1d8152766bcf6ed5cbb49e4acf1e00a9864012473da43f33b4b26346ad2114e8e0ed02767451a4f3282c547d6081
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
