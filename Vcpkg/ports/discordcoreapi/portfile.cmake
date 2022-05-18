if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF bf472d224edf8aac936339d4b21175afc6cc40d8
	SHA512 2bf05b6a3b4aa96abdaa0a316e6a97031c760ed617830441d6a1d5759b2a8474cc3f777f6bb36e27b45ed5867a21c38da87a09bbcd9f53fe7b400256dcb31e1e
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
