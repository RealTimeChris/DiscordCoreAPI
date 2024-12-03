if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO RealTimeChris/DiscordCoreAPI
    REF "v${VERSION}"
    SHA512 89878716432457bea78e5fc6c29f223f78a733897993596db278fd32afd45b31b0b665f478cb80938527da9325d5afa964d52ea405d1c4be46c79ed690038790
    HEAD_REF main
)

# discordcoreapi consumes extreme amounts of memory (>9GB per .cpp file). With our default
# concurrency values this causes hanging and/or OOM killing on Linux build machines and
# warnings on the Windows machines like:
# #[warning]Free memory is lower than 5%; Currently used: 99.99%
# #[warning]Free memory is lower than 5%; Currently used: 99.99%
# #[warning]Free memory is lower than 5%; Currently used: 99.99%
# Cut the requested concurrency in quarter to avoid this.
if(VCPKG_CONCURRENCY GREATER 4)
    math(EXPR VCPKG_CONCURRENCY "${VCPKG_CONCURRENCY} / 4")
endif()

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
)

vcpkg_cmake_install()

vcpkg_cmake_config_fixup(NO_PREFIX_CORRECTION)

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include" "${CURRENT_PACKAGES_DIR}/debug/share")

if(VCPKG_LIBRARY_LINKAGE STREQUAL "static")
    file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/bin" "${CURRENT_PACKAGES_DIR}/debug/bin")
endif()

vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/License.md")