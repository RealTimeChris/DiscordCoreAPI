if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "DiscordCoreAPI only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
	REF 903554bab5a185e7ad698f2210d8c8f153f77c1b
	SHA512 f979bdd8080251a08716701cbfc07cb298e6b549ed8293ca8e30de88640f36c6822d4dd5e38475220f5b8646afe578f5abc0e5570ed41e338e1614b9ae5b3deb
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
