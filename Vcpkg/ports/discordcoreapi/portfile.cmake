if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF f124bdbc16f0ae65c38983eb0a0b22d3779a6e48
	SHA512 bf535b95a4462dbe699b500b591735eb60f3f263d8d11bd4dd6c516ac4c8f5f347afa8a259140d621c7d1f5585f9224eda981b0053d6923b7bac44a9bcf679cc
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
