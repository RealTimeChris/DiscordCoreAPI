if (WIN32)
	set(LIBRARY_SUFFIX ".lib")
	set(LIBRARY_PREFIX "")
elseif(UNIX)
	set(LIBRARY_SUFFIX ".a")
	set(LIBRARY_PREFIX "lib")
endif()

find_curl("${CURL_RELEASE_ROOT}" "${CURL_DEBUG_ROOT}" "${CURL_INCLUDE_DIR}")
find_ffmpeg("${FFMPEG_RELEASE_ROOT}" "${FFMPEG_DEBUG_ROOT}" "${FFMPEG_INCLUDE_DIR}")
find_glib("${GLIB_RELEASE_ROOT}" "${GLIB_DEBUG_ROOT}" "${GLIB_INCLUDE_DIR}")
find_intl("${GLIB_RELEASE_ROOT}" "${GLIB_DEBUG_ROOT}" "${GLIB_INCLUDE_DIR}")
find_libz("${LIBZ_RELEASE_ROOT}" "${LIBZ_DEBUG_ROOT}" "${LIBZ_INCLUDE_DIR}")
find_openssl("${OPENSSL_RELEASE_ROOT}" "${OPENSSL_DEBUG_ROOT}" "${OPENSSL_INCLUDE_DIR}")
find_opus("${OPUS_RELEASE_ROOT}" "${OPUS_DEBUG_ROOT}" "${OPUS_INCLUDE_DIR}")
find_sodium("${SODIUM_RELEASE_ROOT}" "${SODIUM_DEBUG_ROOT}" "${SODIUM_INCLUDE_DIR}")
find_nlohmann_json("${NLOHMANN_INCLUDE_DIR}")

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

target_link_libraries(
	DiscordCoreAPI PUBLIC
	$<$<TARGET_EXISTS:CURL::Curl>:CURL::Curl>
	$<$<TARGET_EXISTS:FFMPEG::AVCodec>:FFMPEG::AVCodec>
	$<$<TARGET_EXISTS:FFMPEG::AVFormat>:FFMPEG::AVFormat>
	$<$<TARGET_EXISTS:FFMPEG::AVUtil>:FFMPEG::AVUtil>
	$<$<TARGET_EXISTS:FFMPEG::SWResample>:FFMPEG::SWResample>
	$<$<TARGET_EXISTS:GLIB::Glib>:GLIB::Glib>
	$<$<TARGET_EXISTS:INTL::Intl>:INTL::Intl>
	$<$<TARGET_EXISTS:LIBZ::Libz>:LIBZ::Libz>
	$<$<TARGET_EXISTS:OPENSSL::Crypto>:OPENSSL::Crypto>
	$<$<TARGET_EXISTS:OPENSSL::Ssl>:OPENSSL::Ssl>
	$<$<TARGET_EXISTS:OPUS::Opus>:OPUS::Opus>
	$<$<TARGET_EXISTS:SODIUM::Sodium>:SODIUM::Sodium>
	$<$<TARGET_EXISTS:NLOHMANN::Json>:NLOHMANN::Json>
)

if (VCPKG_INSTALLED_DIR)
	set(GLIB_BASE_DIR "${VCPKG_INSTALLED_DIR}")	
else()
	set(GLIB_BASE_DIR "${GLIB_INCLUDE_DIR}")
endif()

target_include_directories(
	DiscordCoreAPI PUBLIC
	"$<$<NOT:$<STREQUAL:$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>,${VCPKG_INSTALLED_DIR}/include>>:$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>>"
	"$<$<NOT:$<STREQUAL:$<BUILD_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>,${VCPKG_INSTALLED_DIR}/include>>:$<BUILD_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>>"
	PRIVATE
	"$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Include/>"
	"${GLIB_BASE_DIR}/lib/glib-2.0/include"
	"${GLIB_BASE_DIR}/include/glib-2.0"
	"${GLIB_BASE_DIR}/lib/glib-2.0/"
	"${GLIB_BASE_DIR}/include/"
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
	RUNTIME_OUTPUT_DIRECTORY_RELEASE "$<INSTALL_INTERFACE:$<$<PLATFORM_ID:Windows>:$<INSTALL_PREFIX>/bin/>>"
	RUNTIME_OUTPUT_DIRECTORY_DEBUG "$<INSTALL_INTERFACE:$<$<PLATFORM_ID:Windows>:$<INSTALL_PREFIX>/debug/bin/>>"
	ARCHIVE_OUTPUT_DIRECTORY_RELEASE "$<INSTALL_INTERFACE:$<$<PLATFORM_ID:Windows>:$<INSTALL_PREFIX>/lib/>>"
	ARCHIVE_OUTPUT_DIRECTORY_DEBUG "$<INSTALL_INTERFACE:$<$<PLATFORM_ID:Windows>:$<INSTALL_PREFIX>/debug/lib/>>"
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
	RUNTIME DESTINATION "$<IF:$<CONFIG:Debug>,$<IF:$<STREQUAL:${_VCPKG_INSTALLED_DIR},>,debug/bin,bin>,bin>"
	ARCHIVE DESTINATION "$<IF:$<CONFIG:Debug>,$<IF:$<STREQUAL:${_VCPKG_INSTALLED_DIR},>,debug/lib,lib>,lib>"
	PUBLIC_HEADER DESTINATION "${DISCORDCOREAPI_INCLUDE_DIR}"
	CONFIGURATIONS Release | Debug
)

install(
	EXPORT "${EXPORTED_TARGETS_NAME}"
	FILE "${EXPORTED_TARGETS_FILENAME}"
	NAMESPACE DiscordCoreAPI::
	DESTINATION "share/discordcoreapi"
	CONFIGURATIONS Release | Debug
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
	DESTINATION "share/discordcoreapi"
	CONFIGURATIONS Release | Debug
)

install(
	FILES "$<$<PLATFORM_ID:Windows>:$<TARGET_PDB_FILE:DiscordCoreAPI>>"
	DESTINATION "$<IF:$<CONFIG:Debug>,$<IF:$<STREQUAL:${_VCPKG_INSTALLED_DIR},>,./debug/bin,./bin>,./bin>"
	CONFIGURATIONS Release | Debug
	OPTIONAL
)

message(STATUS "Copying a file...")
install(FILES "${CMAKE_SOURCE_DIR}/Certs/SoundCloudCert.pem" DESTINATION "share/discordcoreapi/data")
message(STATUS "Copying a file...")
install(FILES "${CMAKE_SOURCE_DIR}/Certs/GoogleCert.pem" DESTINATION "share/discordcoreapi/data")
message(STATUS "Copying a file...")
install(FILES "${CMAKE_SOURCE_DIR}/Certs/DiscordCert.pem" DESTINATION "share/discordcoreapi/data")
