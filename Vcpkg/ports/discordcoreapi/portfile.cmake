if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 54dccc664a9b9d017166d723c699eeef6c82cb1f
	SHA512 1310209aeeda3f5db492c6e7b81ae3e3528dd129c9ed1d70aeb72f1713b4f17fef124459ef948c251b735a9d850442b7aea4cf52cc1e8c7ab54c09d41d1efcfa
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
