if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 589867cdf29a5a28d8e1b05aa2d2a2a902f257f8
	SHA512 ae49be8eae468a4b322d95827aeb120382ff3df47d611793d9c960cdfa4f27e903155b6a23aa01a7eb9afbaa445b585d294f8588b93bf3f82ddc938d0224dbb4
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