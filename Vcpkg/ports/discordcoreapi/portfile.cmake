if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF bc5bc9233e540e9c34d494f2f3f66a95d9fdf9c0
	SHA512 86eb7aa391d16a079f249ab37a16dae447ae96a60865665fde2db0ba1b28938a255bc7f1165ebc26541101eced632623368585e3d8bc0b01cc0c16296892542b
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
