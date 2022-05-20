if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 1bc5f63aa7242e1e2ac0bcfdcd537fac3fc92cdf
	SHA512 fdc3a08c5000b6f61ecbad5b013d3c4ad490559f5810c8533ef4f6a0827b498b92233f745dc558a26eb7658de05c36d54c89464530318e4dde20fd258b2d3e95
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
