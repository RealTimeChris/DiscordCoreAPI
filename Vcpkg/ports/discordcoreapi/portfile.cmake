if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

# Set the number of parallel jobs to 1
if(NOT DEFINED ENV{VCPKG_MAX_CONCURRENCY})
    set(ENV{VCPKG_MAX_CONCURRENCY} 1)  # Set this to any desired limit (e.g., 4)
endif()

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO RealTimeChris/DiscordCoreAPI
    REF "v${VERSION}"
    SHA512 2c162d2bcf67b8acb1fe124f6d6be15854a8d293ea1d9418b24cdeee25610fbd93a45c7203646f3ef6a58e5b69a36b9f3b63989c33484635c8db17f5b52b98af
    HEAD_REF main
)

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    DISABLE_PARALLEL_CONFIGURE
)

vcpkg_cmake_install()

vcpkg_cmake_config_fixup(NO_PREFIX_CORRECTION)

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include" "${CURRENT_PACKAGES_DIR}/debug/share")

if(VCPKG_LIBRARY_LINKAGE STREQUAL "static")
    file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/bin" "${CURRENT_PACKAGES_DIR}/debug/bin")
endif()

vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/License.md")
