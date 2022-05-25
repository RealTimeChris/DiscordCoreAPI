if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 03a09742acc5fe2f6c5e5115ba90e9542a963c8c
	SHA512 e00d6231455fac2814c68be5639d9b36011536211c41a950cab7c104e6be14cd02c81fc61a9fb3dba172c1d73dce81a0a4401c227f4fd4129e0942d0e776bac1
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
