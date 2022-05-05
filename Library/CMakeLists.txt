generate_product_version(
	PRODUCT_INFO
	NAME "${LIB_NAME}"
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
elseif (UNIX)
	add_library("${LIB_NAME}" STATIC "${SOURCES}" "${PRODUCT_INFO}")
	find_package(Threads REQUIRED)
endif()

add_library("${PROJECT_NAME}::${LIB_NAME}" ALIAS "${LIB_NAME}")

file(GLOB HEADERS CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/Include/discordcoreapi/*.hpp")

set_target_properties(
	"${LIB_NAME}" PROPERTIES
	PUBLIC_HEADER "${HEADERS}"
	OUTPUT_NAME "discordcoreapi"
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF
	RUNTIME_OUTPUT_DIRECTORY "$<INSTALL_INTERFACE:$<IF:$<CONFIG:Debug>,$<INSTALL_PREFIX>/debug/bin/,$<INSTALL_PREFIX>/bin/>>$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Output Files/$<CONFIG>>"
	ARCHIVE_OUTPUT_DIRECTORY "$<INSTALL_INTERFACE:$<IF:$<CONFIG:Debug>,$<INSTALL_PREFIX>/debug/lib/,$<INSTALL_PREFIX>/lib/>>$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Output Files/$<CONFIG>>"
	PDB_OUTPUT_DIRECTORY "$<INSTALL_INTERFACE:$<IF:$<CONFIG:Debug>,$<INSTALL_PREFIX>/debug/bin/,$<INSTALL_PREFIX>/bin/>>$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Output Files/$<CONFIG>>"
)

if (EXISTS "${VCPKG_INSTALLED_DIR}")
	set(GLIB_INCLUDE_DIR "${VCPKG_INSTALLED_DIR}/include")
endif()

target_include_directories(
	"${LIB_NAME}" PUBLIC
	"$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>"
	PRIVATE
	"$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Include/>"
	"${GLIB_INCLUDE_DIR}/glib-2.0"
	"${GLIB_INCLUDE_DIR}/../lib/glib-2.0/include"
)

target_link_libraries(
	"${LIB_NAME}" PUBLIC
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

target_compile_features(
	"${LIB_NAME}" PUBLIC
	"cxx_std_20"
)

if (DEFINED ENV{EnableASAN})
	if(MSVC)
		target_compile_options("${LIB_NAME}" PUBLIC "/fsanitize=address" "/Zi" "/Debug")
		target_link_options("${LIB_NAME}" PUBLIC "/incremental:no" "/Debug")	
	else()
		target_compile_options("${LIB_NAME}" PUBLIC "-fsanitize=address")
		target_link_options("${LIB_NAME}" PUBLIC "-fsanitize=address")
	endif()
endif()

target_compile_options(
	"${LIB_NAME}" PUBLIC
	"$<$<PLATFORM_ID:Windows>:/bigobj>"
	"$<$<PLATFORM_ID:Windows>:/MP>"
	"$<$<PLATFORM_ID:Linux>:-O3>"
)

target_link_options(
	"${LIB_NAME}" PUBLIC
	"$<$<PLATFORM_ID:Windows>:/DEBUG>"
)

set(EXPORTED_TARGETS_NAME "DiscordCoreAPITargets")
set(DISCORDCOREAPI_INCLUDE_DIR "${CMAKE_INSTALL_PREFIX}/include/discordcoreapi")

install(
	TARGETS "${LIB_NAME}"
	EXPORT "${EXPORTED_TARGETS_NAME}"
	RUNTIME DESTINATION "$<IF:$<CONFIG:Debug>,$<IF:$<STREQUAL:${_VCPKG_INSTALLED_DIR},>,debug/bin,bin>,bin>"
	ARCHIVE DESTINATION "$<IF:$<CONFIG:Debug>,$<IF:$<STREQUAL:${_VCPKG_INSTALLED_DIR},>,debug/lib,lib>,lib>"
	PUBLIC_HEADER DESTINATION "${DISCORDCOREAPI_INCLUDE_DIR}"
)

set(EXPORTED_TARGETS_FILE_NAME "${EXPORTED_TARGETS_NAME}.cmake")

install(
	EXPORT "${EXPORTED_TARGETS_NAME}"
	FILE "${EXPORTED_TARGETS_FILE_NAME}"
	NAMESPACE "${LIB_NAME}::"
	DESTINATION "share/discordcoreapi"
)

set(EXPORTED_TARGETS_FILE_PATH "share/discordcoreapi/${EXPORTED_TARGETS_FILE_NAME}")
set(CONFIG_FILE_NAME "DiscordCoreAPIConfig.cmake")
set(RELEASE_PDB_FILE_PATH "${CMAKE_INSTALL_PREFIX}/bin/discordcoreapi.pdb")
set(DEBUG_PDB_FILE_PATH "${CMAKE_INSTALL_PREFIX}/debug/bin/discordcoreapi.pdb")

configure_package_config_file(
	"${CMAKE_SOURCE_DIR}/CMake/${CONFIG_FILE_NAME}.in"
	"${CMAKE_CURRENT_BINARY_DIR}/${CONFIG_FILE_NAME}"
	INSTALL_DESTINATION "share/discordcoreapi"
	PATH_VARS
	EXPORTED_TARGETS_FILE_PATH
	RELEASE_PDB_FILE_PATH
	DEBUG_PDB_FILE_PATH
)

set(VERSION_FILE_NAME "DiscordCoreAPIConfigVersion.cmake")

write_basic_package_version_file(
	"${CMAKE_CURRENT_BINARY_DIR}/${VERSION_FILE_NAME}"
	VERSION "1.0.0"
	COMPATIBILITY AnyNewerVersion
)

install(
	FILES
	"${CMAKE_CURRENT_BINARY_DIR}/${CONFIG_FILE_NAME}"
	"${CMAKE_CURRENT_BINARY_DIR}/${VERSION_FILE_NAME}"
	DESTINATION "${CMAKE_INSTALL_PREFIX}/share/discordcoreapi"
)

install(
	FILES 
	"$<$<PLATFORM_ID:Windows>:$<TARGET_PDB_FILE:${LIB_NAME}>>"
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
