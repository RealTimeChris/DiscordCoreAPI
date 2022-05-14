if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 65cb59c88dfeb71f13dbf31f5dd69578a54540c1
	SHA512 cb68b1225f1f3ce75afe90b81f46f78dd46c1c642cee4bf90c59a8307991f4eb5a193fb688f3ea0db85ba7b525a63a1c281ace64682c25e2691d69ec66a91b82
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
