if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 62603cf5cd40bb8c79bbb594a8248305b47a3462
	SHA512 293c2103d3f829e46c4a5356eb3223e6b423d17df6a915942e0c6b7c84164fc9f8861b4807ae2925fc2fda76a0699ef805f82aa72e37fa5c871b9570335a3cd2
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
