if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF be04dddf89cc447b3857523b4ad0648c36f7c466
	SHA512 b2ecc93796c369a37be04cb9366fed5a6e132c8eb64afeefd1629d20686a6b9d2f69d139aa2638f4eee826c666abe3a7f9e3f039038f52eebb4084d7f245afc3
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