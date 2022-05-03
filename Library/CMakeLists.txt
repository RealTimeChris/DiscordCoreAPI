generate_product_version(
	PRODUCT_INFO
	NAME "DiscordCoreAPI"
	FILE_DESCRIPTION "A Discord bot library written in C++, using asynchronous coroutines."
	ORIGINAL_FILENAME "discordcoreapi.dll"
	COMPANY_NAME "RealTimeChris Inc."
	VERSION_MAJOR "1"
	VERSION_MINOR "0"
	VERSION_PATCH "0"
	VERSION_REVISION "0"
)

file(GLOB SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/Source/*.cpp")

if (WIN32)
	add_library("${LIB_NAME}" SHARED "${SOURCES}" "${PRODUCT_INFO}")
	set(LIBRARY_SUFFIX ".lib")
	set(LIBRARY_PREFIX "")
elseif (UNIX)
	add_library("${LIB_NAME}" STATIC "${SOURCES}" "${PRODUCT_INFO}")
	set(LIBRARY_SUFFIX ".a")
	set(LIBRARY_PREFIX "lib")
	find_package(Threads REQUIRED)
endif()

add_library("${PROJECT_NAME}::${LIB_NAME}" ALIAS "${LIB_NAME}")

if (DEFINED ENV{EnableASAN})
	if(MSVC)
		target_compile_options(DiscordCoreAPI PUBLIC "/fsanitize=address" "/Zi" "/Debug")
		target_link_options(DiscordCoreAPI PUBLIC "/incremental:no" "/Debug")	
	else()
		target_compile_options(DiscordCoreAPI PUBLIC "-fsanitize=address")
		target_link_options(DiscordCoreAPI PUBLIC "-fsanitize=address")
	endif()
endif()

find_curl("${CURL_RELEASE_ROOT}" "${CURL_DEBUG_ROOT}" "${CURL_INCLUDE_DIR}")
find_ffmpeg("${FFMPEG_RELEASE_ROOT}" "${FFMPEG_DEBUG_ROOT}" "${FFMPEG_INCLUDE_DIR}")
find_glib("${GLIB_RELEASE_ROOT}" "${GLIB_DEBUG_ROOT}" "${GLIB_INCLUDE_DIR}")
find_intl("${INTL_RELEASE_ROOT}" "${INTL_DEBUG_ROOT}" "${INTL_INCLUDE_DIR}")
find_libz("${LIBZ_RELEASE_ROOT}" "${LIBZ_DEBUG_ROOT}" "${LIBZ_INCLUDE_DIR}")
find_nlohmann_json("${NLOHMANN_INCLUDE_DIR}")
find_openssl("${OPENSSL_RELEASE_ROOT}" "${OPENSSL_DEBUG_ROOT}" "${OPENSSL_INCLUDE_DIR}")
find_opus("${OPUS_RELEASE_ROOT}" "${OPUS_DEBUG_ROOT}" "${OPUS_INCLUDE_DIR}")
find_sodium("${SODIUM_RELEASE_ROOT}" "${SODIUM_DEBUG_ROOT}" "${SODIUM_INCLUDE_DIR}")

target_link_libraries(
	DiscordCoreAPI PUBLIC
	$<$<TARGET_EXISTS:CURL::Curl>:CURL::Curl>
	$<$<TARGET_EXISTS:FFMPEG::AVFormat>:FFMPEG::AVFormat>
	$<$<TARGET_EXISTS:FFMPEG::AVCodec>:FFMPEG::AVCodec>
	$<$<TARGET_EXISTS:FFMPEG::AVUtil>:FFMPEG::AVUtil>
	$<$<TARGET_EXISTS:FFMPEG::SWResample>:FFMPEG::SWResample>
	$<$<TARGET_EXISTS:GLIB::Glib>:GLIB::Glib>
	$<$<TARGET_EXISTS:INTL::Intl>:INTL::Intl>
	$<$<TARGET_EXISTS:LIBZ::Libz>:LIBZ::Libz>
	$<$<TARGET_EXISTS:NLOHMANN::Json>:NLOHMANN::Json>
	$<$<TARGET_EXISTS:OPENSSL::Ssl>:OPENSSL::Ssl>
	$<$<TARGET_EXISTS:OPENSSL::Crypto>:OPENSSL::Crypto>
	$<$<TARGET_EXISTS:OPUS::Opus>:OPUS::Opus>
	$<$<TARGET_EXISTS:SODIUM::Sodium>:SODIUM::Sodium>
	$<$<TARGET_EXISTS:Threads::Threads>:Threads::Threads>
)

file(GLOB HEADERS CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/Include/discordcoreapi/*.hpp")

set_target_properties(
	DiscordCoreAPI PROPERTIES
	PUBLIC_HEADER "${HEADERS}"
	OUTPUT_NAME "discordcoreapi"
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF
	RUNTIME_OUTPUT_DIRECTORY "$<INSTALL_INTERFACE:$<IF:$<CONFIG:Debug>,$<INSTALL_PREFIX>/debug/bin/,$<INSTALL_PREFIX>/bin/>>$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Output Files/$<CONFIG>>"
	ARCHIVE_OUTPUT_DIRECTORY "$<INSTALL_INTERFACE:$<IF:$<CONFIG:Debug>,$<INSTALL_PREFIX>/debug/lib/,$<INSTALL_PREFIX>/lib/>>$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Output Files/$<CONFIG>>"
	PDB_OUTPUT_DIRECTORY_RELEASE "$<INSTALL_INTERFACE:$<IF:$<CONFIG:Debug>,$<INSTALL_PREFIX>/debug/bin/,$<INSTALL_PREFIX>/bin/>>$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Output Files/$<CONFIG>>"
)

if (EXISTS "${VCPKG_INSTALLED_DIR}")
	set(GLIB_INCLUDE_DIR "${VCPKG_INSTALLED_DIR}")
endif()

target_include_directories(
	DiscordCoreAPI PUBLIC
	"$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>"
	PRIVATE
	"$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Include/>"
	"${GLIB_INCLUDE_DIR}/lib/glib-2.0/include"
	"${GLIB_INCLUDE_DIR}/include/glib-2.0"
	"${GLIB_INCLUDE_DIR}/lib/glib-2.0/"
	"${GLIB_INCLUDE_DIR}/include/"
)

target_link_options(
	DiscordCoreAPI PUBLIC
	"$<$<PLATFORM_ID:Windows>:/DEBUG>"
)

target_compile_options(
	DiscordCoreAPI PUBLIC
	"$<$<PLATFORM_ID:Windows>:/bigobj>"
	"$<$<PLATFORM_ID:Windows>:/MP>"
	"$<$<PLATFORM_ID:Linux>:-O3>"
)

target_compile_features(
	DiscordCoreAPI PUBLIC
	"cxx_std_20"
)

set(EXPORTED_TARGETS_NAME "DiscordCoreAPITargets")
set(DISCORDCOREAPI_INCLUDE_DIR "${CMAKE_INSTALL_PREFIX}/include/discordcoreapi")

install(
	TARGETS DiscordCoreAPI
	EXPORT "${EXPORTED_TARGETS_NAME}"
	RUNTIME DESTINATION "$<IF:$<CONFIG:Debug>,$<IF:$<STREQUAL:${_VCPKG_INSTALLED_DIR},>,debug/bin,bin>,bin>"
	ARCHIVE DESTINATION "$<IF:$<CONFIG:Debug>,$<IF:$<STREQUAL:${_VCPKG_INSTALLED_DIR},>,debug/lib,lib>,lib>"
	PUBLIC_HEADER DESTINATION "${DISCORDCOREAPI_INCLUDE_DIR}"
)

set(EXPORTED_TARGETS_FILE_NAME "${EXPORTED_TARGETS_NAME}.cmake")

install(
	EXPORT "${EXPORTED_TARGETS_NAME}"
	FILE "${EXPORTED_TARGETS_FILE_NAME}"
	NAMESPACE DiscordCoreAPI::
	DESTINATION "share/discordcoreapi"
)

set(EXPORTED_TARGETS_FILE_PATH "share/discordcoreapi/${EXPORTED_TARGETS_FILE_NAME}")
set(CONFIG_FILE_NAME "DiscordCoreAPIConfig.cmake")
set(RELEASE_PDB_FILE_PATH "${CMAKE_INSTALL_PREFIX}/bin/discordcoreapi.pdb")
set(DEBUG_PDB_FILE_PATH "${CMAKE_INSTALL_PREFIX}/debug/bin/discordcoreapi.pdb")

configure_package_config_file(
	"${CMAKE_SOURCE_DIR}/CMake/${CONFIG_FILE_NAME}.in"
	"${CMAKE_INSTALL_PREFIX}/share/discordcoreapi/${CONFIG_FILE_NAME}"
	INSTALL_DESTINATION "share/discordcoreapi"
	PATH_VARS
	EXPORTED_TARGETS_FILE_PATH
	RELEASE_PDB_FILE_PATH
	DEBUG_PDB_FILE_PATH
)

set(VERSION_FILE_NAME "DiscordCoreAPIConfigVersion.cmake")

write_basic_package_version_file(
	"${CMAKE_INSTALL_PREFIX}/share/discordcoreapi/${VERSION_FILE_NAME}"
	VERSION "1.0.0"
	COMPATIBILITY AnyNewerVersion
)

install(
	FILES 
	"$<$<PLATFORM_ID:Windows>:$<TARGET_PDB_FILE:DiscordCoreAPI>>"
	DESTINATION "$<IF:$<CONFIG:Debug>,$<IF:$<STREQUAL:${_VCPKG_INSTALLED_DIR},>,debug/bin,bin>,bin>"
	OPTIONAL
)

install(
	FILES 
	"${CMAKE_SOURCE_DIR}/Certs/DiscordCert.pem"
	"${CMAKE_SOURCE_DIR}/Certs/GoogleCert.pem"
	"${CMAKE_SOURCE_DIR}/Certs/SoundCloudCert.pem" 
	DESTINATION "share/discordcoreapi/data"
)
