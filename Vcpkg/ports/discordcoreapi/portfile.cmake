if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
<<<<<<< Updated upstream
	REF 1aa5c7c0e99dc21144c2672c56a341221e0bfc3a
	SHA512 0509f2c4a72823a652875abbf6389550e871991f13ce574983de5d50d8dead33450fb86a425114d513060a4ef0084aa39f7b548b63010c3474ada8af4dc92625
=======
	REF cfc88fa5b830809206a4646cb07c569b2ad4957a
	SHA512 340388101063a9c869322e5889742c1233528ff9124ab8c156d4522de1dfe47d4b384cb04bbd62d8631e40fed981d8e229409929e45967a5c7f2531b0b28a8ba
>>>>>>> Stashed changes
	HEAD_REF main
)

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
)

vcpkg_cmake_install()

vcpkg_cmake_config_fixup(NO_PREFIX_CORRECTION)

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include" "${CURRENT_PACKAGES_DIR}/debug/share")

if(VCPKG_LIBRARY_LINKAGE STREQUAL "static")
    file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/bin" "${CURRENT_PACKAGES_DIR}/debug/bin")
endif()

file(
	INSTALL "${SOURCE_PATH}/License.md"
	DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}"
	RENAME copyright
)
