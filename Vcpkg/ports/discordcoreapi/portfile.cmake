if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 4333e36394a4d81cbbf6b9cd894bd9c776cf474f
	SHA512 80c95521d00c8b072c481cafadca8f72ec2331dad962928fad2277e6d0e348bfc221f78bee23ff59226a9e51a7cf01e623a946f6924ca526c6ad0def4ead64b7
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
