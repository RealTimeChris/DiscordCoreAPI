if (WIN32)
	set(LIBRARY_SUFFIX ".lib")
	set(LIBRARY_PREFIX "")
elseif(UNIX)
	set(LIBRARY_SUFFIX ".a")
	set(LIBRARY_PREFIX "lib")
endif()

find_curl("${CURL_RELEASE_ROOT}" "${CURL_DEBUG_ROOT}")
find_ffmpeg("${FFMPEG_RELEASE_ROOT}" "${FFMPEG_DEBUG_ROOT}")
find_glib("${GLIB_RELEASE_ROOT}" "${GLIB_DEBUG_ROOT}")
find_intl("${GLIB_RELEASE_ROOT}" "${GLIB_DEBUG_ROOT}")
find_libz("${LIBZ_RELEASE_ROOT}" "${LIBZ_DEBUG_ROOT}")
find_openssl("${OPENSSL_RELEASE_ROOT}" "${OPENSSL_DEBUG_ROOT}")
find_opus("${OPUS_RELEASE_ROOT}" "${OPUS_DEBUG_ROOT}")
find_sodium("${SODIUM_RELEASE_ROOT}" "${SODIUM_DEBUG_ROOT}")

generate_product_version(
	PRODUCT_INFO
	NAME "DiscordCoreAPI"
	ICON "${CMAKE_SOURCE_DIR}/Documentation/Images/TheLogo.ico"
	FILE_DESCRIPTION "A Discord bot library written in C++, using asynchronous coroutines."
	ORIGINAL_FILENAME "discordcoreapi.dll"
	COMPANY_NAME "RealTimeChris Inc."
	VERSION_MAJOR "1"
	VERSION_MINOR "0"
	VERSION_PATCH "0"
	VERSION_REVISION "0"
)

file(GLOB HEADERS "${CMAKE_SOURCE_DIR}/Include/discordcoreapi/*.hpp")
file(GLOB SOURCES "${CMAKE_SOURCE_DIR}/Source/*.cpp")

if (WIN32)
	add_library(DiscordCoreAPI SHARED "${SOURCES}" "${PRODUCT_INFO}")
elseif (UNIX)
	add_library(DiscordCoreAPI STATIC "${SOURCES}" "${PRODUCT_INFO}")
endif()

add_library(DiscordCoreAPI::DiscordCoreAPI ALIAS DiscordCoreAPI)

foreach (LIBRARY "${RELEASE_LIBRARIES}") 
	if (LIBRARY)
		target_link_libraries(
			DiscordCoreAPI PUBLIC
			"$<$<CONFIG:Release>:${LIBRARY}>"
		)
	endif()
endforeach()

foreach (LIBRARY "${DEBUG_LIBRARIES}")
	if (LIBRARY)
		target_link_libraries(
			DiscordCoreAPI PUBLIC
			"$<$<CONFIG:Debug>:${LIBRARY}>"
		)
	endif()
endforeach()

if (VCPKG_INSTALLED_DIR)
	set(GLIB_BASE_DIR "${VCPKG_INSTALLED_DIR}")	
else()
	set(GLIB_BASE_DIR "${GLIB_INCLUDE_DIR}")
endif()

target_include_directories(
	DiscordCoreAPI PUBLIC
	"$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>"
	"$<$<NOT:$<STREQUAL:${NLOHMANN_INCLUDE_DIR},${VCPKG_INSTALLED_DIR}/include>>:${NLOHMANN_INCLUDE_DIR}>"
	"$<$<NOT:$<STREQUAL:${CURL_INCLUDE_DIR},${VCPKG_INSTALLED_DIR}/include>>:${CURL_INCLUDE_DIR}>"
	"$<$<NOT:$<STREQUAL:${FFMPEG_INCLUDE_DIR},${VCPKG_INSTALLED_DIR}/include>>:${FFMPEG_INCLUDE_DIR}>"
	"$<$<NOT:$<STREQUAL:${GLIB_INCLUDE_DIR},${VCPKG_INSTALLED_DIR}/include>>:${GLIB_INCLUDE_DIR}>"
	"$<$<NOT:$<STREQUAL:${LIBZ_INCLUDE_DIR},${VCPKG_INSTALLED_DIR}/include>>:${LIBZ_INCLUDE_DIR}>"
	"$<$<NOT:$<STREQUAL:${OPENSSL_INCLUDE_DIR},${VCPKG_INSTALLED_DIR}/include>>:${OPENSSL_INCLUDE_DIR}>"
	"$<$<NOT:$<STREQUAL:${OPUS_INCLUDE_DIR},${VCPKG_INSTALLED_DIR}/include>>:${OPUS_INCLUDE_DIR}>"
	"$<$<NOT:$<STREQUAL:${SODIUM_INCLUDE_DIR},${VCPKG_INSTALLED_DIR}/include>>:${SODIUM_INCLUDE_DIR}>"
	PRIVATE
	"$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Include/>"
	"${GLIB_BASE_DIR}/lib/glib-2.0/include"
	"${GLIB_BASE_DIR}/include/glib-2.0"
	"${GLIB_BASE_DIR}/lib/glib-2.0/"
)

target_compile_options(
	DiscordCoreAPI PUBLIC
	"$<$<PLATFORM_ID:Windows>:$<$<CONFIG:Release>:/O2>>"
	"$<$<PLATFORM_ID:Windows>:$<$<CONFIG:Debug>:/MDd>>"
	"$<$<PLATFORM_ID:Linux>:-std=c++2a>"
	"$<$<PLATFORM_ID:Windows>:/bigobj>"
	"$<$<PLATFORM_ID:Linux>:-pthread>"
	"$<$<PLATFORM_ID:Windows>:/W4>"
	"$<$<PLATFORM_ID:Windows>:/WX>"
	"$<$<PLATFORM_ID:Windows>:/MP>"
	"$<$<PLATFORM_ID:Linux>:-O3>"
)

target_compile_features(
	DiscordCoreAPI PUBLIC
	"$<$<PLATFORM_ID:Windows>:cxx_std_23>"
	"$<$<PLATFORM_ID:Linux>:cxx_std_20>"
)

set_target_properties(
	DiscordCoreAPI PROPERTIES
	PUBLIC_HEADER "${HEADERS}"
	OUTPUT_NAME "discordcoreapi"
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF
	LINKER_LANGUAGE CXX
	IMPORTED_IMPLIB_RELEASE "$<INSTALL_INTERFACE:$<$<PLATFORM_ID:Windows>:$<INSTALL_PREFIX>/lib/discordcoreapi.lib>>"
	IMPORTED_IMPLIB_DEBUG "$<INSTALL_INTERFACE:$<$<PLATFORM_ID:Windows>:$<INSTALL_PREFIX>/debug/lib/discordcoreapi.lib>>"
	IMPORTED_LOCATION_RELEASE "$<INSTALL_INTERFACE:$<$<PLATFORM_ID:Windows>:$<INSTALL_PREFIX>/bin/discordcoreapi.dll>>"
	IMPORTED_LOCATION_DEBUG "$<INSTALL_INTERFACE:$<$<PLATFORM_ID:Windows>:$<INSTALL_PREFIX>/debug/bin/discordcoreapi.dll>>"
	IMPORTED_LOCATION_RELEASE "$<INSTALL_INTERFACE:$<$<PLATFORM_ID:Linux>:$<INSTALL_PREFIX>/lib/libdiscordcoreapi.a>>"
	IMPORTED_LOCATION_DEBUG "$<INSTALL_INTERFACE:$<$<PLATFORM_ID:Linux>:$<INSTALL_PREFIX>/debug/lib/discordcoreapi.a>>"
	IMPORTED_CONFIGURATIONS "Release;Debug"
)

set(EXPORTED_TARGETS_NAME "DiscordCoreAPITargets")
set(EXPORTED_TARGETS_FILENAME "${EXPORTED_TARGETS_NAME}.cmake")
set(CONFIG_FILENAME "DiscordCoreAPIConfig.cmake")
set(DISCORDCOREAPI_INCLUDE_DIR "${CMAKE_INSTALL_PREFIX}/include/discordcoreapi")

install(
	TARGETS DiscordCoreAPI
	EXPORT "${EXPORTED_TARGETS_NAME}"
	CONFIGURATIONS Release | Debug
	RUNTIME DESTINATION "$<IF:$<CONFIG:Debug>,$<IF:$<STREQUAL:${_VCPKG_INSTALLED_DIR},>,${CMAKE_INSTALL_PREFIX}/debug/bin,${CMAKE_INSTALL_PREFIX}/bin>,${CMAKE_INSTALL_PREFIX}/bin>"
	ARCHIVE DESTINATION "$<IF:$<CONFIG:Debug>,$<IF:$<STREQUAL:${_VCPKG_INSTALLED_DIR},>,${CMAKE_INSTALL_PREFIX}/debug/lib,${CMAKE_INSTALL_PREFIX}/lib>,${CMAKE_INSTALL_PREFIX}/lib>"
	PUBLIC_HEADER DESTINATION "${DISCORDCOREAPI_INCLUDE_DIR}"
)

install(
	EXPORT "${EXPORTED_TARGETS_NAME}"
	FILE "${EXPORTED_TARGETS_FILENAME}"
	NAMESPACE DiscordCoreAPI::
	DESTINATION "${CMAKE_INSTALL_PREFIX}/share/discordcoreapi"
)

configure_package_config_file(
	"CMake/${CONFIG_FILENAME}.in"
	"${CONFIG_FILENAME}"
	INSTALL_DESTINATION "${CMAKE_CURRENT_BINARY_DIR}"
	PATH_VARS
	DISCORDCOREAPI_INCLUDE_DIR
)

install(
	FILES "${CMAKE_CURRENT_BINARY_DIR}/${CONFIG_FILENAME}" 
	DESTINATION "${CMAKE_INSTALL_PREFIX}/share/discordcoreapi"
)

install(
	FILES "$<$<PLATFORM_ID:Windows>:$<TARGET_PDB_FILE:DiscordCoreAPI>>"
	DESTINATION "$<IF:$<CONFIG:Debug>,$<IF:$<STREQUAL:${_VCPKG_INSTALLED_DIR},>,${CMAKE_INSTALL_PREFIX}/debug/bin,${CMAKE_INSTALL_PREFIX}/bin>,${CMAKE_INSTALL_PREFIX}/bin>"
	OPTIONAL
)

message(STATUS "Copying a file...")
install(FILES "${CMAKE_SOURCE_DIR}/Certs/SoundCloudCert.pem" DESTINATION "share/discordcoreapi/data")
message(STATUS "Copying a file...")
install(FILES "${CMAKE_SOURCE_DIR}/Certs/GoogleCert.pem" DESTINATION "share/discordcoreapi/data")
message(STATUS "Copying a file...")
install(FILES "${CMAKE_SOURCE_DIR}/Certs/DiscordCert.pem" DESTINATION "share/discordcoreapi/data")
