if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 6715288ec3d299a903f2ccc7cbba6b53d6457050
	SHA512 97a04c9f69e37d51244ee7f5c74d58d19c42db0ac0184e041d95511a25f5d23b8551285c89d64d27246911d7c8c969ca6ba5e5efeb55a75ed55fcdeb21e3f9bc
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
