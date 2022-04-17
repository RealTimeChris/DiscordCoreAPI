if(VCPKG_TARGET_IS_LINUX)
    message(WARNING "Discordcoreapi only supports g++ 11 on linux.")
endif()

vcpkg_from_github(
	OUT_SOURCE_PATH SOURCE_PATH
	REPO RealTimeChris/DiscordCoreAPI
<<<<<<< HEAD
	REF 495926ea175282e65a8b2f8e9e2d41f16e505594
	SHA512 6c83f2144effef74b0b76631051da7629abdd3bdc611d44d29c573a27e28bdebe3c4bc731dd29acf19454c7f074c5cf8899256692b5835439429e68d06f20c68
=======
<<<<<<< HEAD
	REF bd0c81dbfd73ee642959fba9ce46699f100098db
	SHA512 29384839ff98cf1d9385729a509aad6f4d6df18a48e51fc0887e804d98d43d77f960b11ccd850fe63da70334f01738f234178263de3c4c171ef48fc3d869998d
=======
	REF 0148569a9f427f1c8f6c3255447a9602bd03c7cb
	SHA512 7884c1e5b4557660bbf8f3a8e2c7523e864ca155e4c9f2271905bb4744f364274911f0e05abe2179413155923b2adaebf94ee135ecf60246dac975b45a96d1c6
>>>>>>> main
>>>>>>> Current-Vcpkg
	HEAD_REF main
)

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
)

vcpkg_cmake_install()

vcpkg_cmake_config_fixup(NO_PREFIX_CORRECTION)

vcpkg_fixup_pkgconfig()

vcpkg_copy_pdbs()

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include" "${CURRENT_PACKAGES_DIR}/debug/share")

if(VCPKG_LIBRARY_LINKAGE STREQUAL "static")
    file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/bin" "${CURRENT_PACKAGES_DIR}/debug/bin")
endif()

file(
	INSTALL "${SOURCE_PATH}/License.md"
	DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}"
	RENAME copyright
)
