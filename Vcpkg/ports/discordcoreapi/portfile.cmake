if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF cf29901ff7e24dba9f2ad72f30dd859d3e891753
	SHA512 ce5eb6ec69211ec3aad794ec3442cd6469eddfc08827e56e85857a944dff52c7a39c3d26bf92f08cad9a35f8e77f3094851eca736845471aea1cb118fc1386ac
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
