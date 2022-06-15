if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 3d114e67d92d9f2f179bdbeb1e22226f58bee7e3
	SHA512 09df62a1ed2a87b74070423d277b090e01b65103fb6f274d010c8e33882bbb4fd65f3ce63f8b4a751ab073e8077ad0264713b9b6bf8e16c5e9d872831c2b9cda
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