if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 48e4aefc18dc833d031ec35d514bd4817116c1b7
	SHA512 38b0968c191fa7ac7ce9b71341e3654b697ea90e655b7d6e6c1ab663d181b755c55cb95569423ed0edda0b4bbeee158bbae52a5891eb4e880ced0776faaf18bc
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
