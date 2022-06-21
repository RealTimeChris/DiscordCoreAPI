if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 038d48c973079da7de39ae68a96ceff376ab5839
	SHA512 9d62868d5f1806ce4dc4e418890cf192fc3b151fc8c61a154e1ae397cb3063ae3f1a2ba08f262b5c9e79c58d5d6d7bddf101ad5ddb51afb469b46e8ed5da16f2
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