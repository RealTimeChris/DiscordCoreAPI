if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 76f7a50c3d0de33c3eb94b000d9787e5dbe14fda
	SHA512 23cbe5afabd5ea2825f00350f8b717aca80a7b44eb5d3dfef0e013bd77323bb8c9f72520384ca0415af555a677d6d8c7ea6dfa777b04cac2d359dcacdce030ba
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
