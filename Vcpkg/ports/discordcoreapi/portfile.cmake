if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 2a127f398cdf5cd2f85897eae8b999bc8235a2d3
	SHA512 038067e1186f157442d33800663dffa19197c958020e316e90100fdecd213c27f560aa6e5fcd01455578eab74ee057b7b10286fbe40a151ec46362de8e7e32c8
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