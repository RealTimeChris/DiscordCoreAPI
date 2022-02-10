if (UNIX)
set(ENV{CMAKE_TOOLCHAIN_FILE} "${VCPKG_BASE_DIR}/scripts/buildsystems/vcpkg.cmake")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../Linux/${CMAKE_BUILD_TYPE}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../Linux/${CMAKE_BUILD_TYPE}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../Linux/${CMAKE_BUILD_TYPE}")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(VCPKG_INSTALLED_DIR "${VCPKG_BASE_DIR}/installed/x64-linux")
add_compile_options(-std=gnu++2a)
add_compile_options(-fcoroutines)
add_compile_options(-lpthread)
include_directories("../Include")
include_directories("../Source")
include_directories("${VCPKG_INSTALLED_DIR}/lib/glib-2.0/include")
include_directories("${VCPKG_INSTALLED_DIR}/include/glib-2.0/")
include_directories("${VCPKG_INSTALLED_DIR}/include")

set(OPENSSL_ROOT_DIR "${VCPKG_INSTALLED_DIR}/lib")
set(OEPNSSL_CRYPTO_LIBRARY "${VCPKG_INSTALLED_DIR}/lib/libcrypto.a")
set(OEPNSSL_SSL_LIBRARY "${VCPKG_INSTALLED_DIR}/lib/libssl.a")
set(ZLIB_LIBRARY "${VCPKG_INSTALLED_DIR}/lib/libz.a")
set(ZLIB_INCLUDE_DIR "${VCPKG_INSTALLED_DIR}/include")
set(LIBCURL_DIR "${VCPKG_INSTALLED_DIR}/share/curl")
find_package(LIBCURL REQUIRED)
set(FFMPEG_DIR "${VCPKG_INSTALLED_DIR}/share/ffmpeg")
find_package(FFMPEG REQUIRED)

add_executable(DiscordCoreAPI-Bot "main.cpp" "${VCPKG_INSTALLED_DIR}/include/cpp-base64/base64.cpp" "../Include/Index.hpp" "../Source/ApplicationCommandEntities.cpp" "../Source/AudioDecoder.cpp"
	"../Source/AudioEncoder.cpp" "../Source/ChannelEntities.cpp"
	"../Source/CommandController.cpp" "../Source/DataParsingFunctions.cpp"
	"../Source/DiscordCoreClient.cpp" "../Source/DiscordCoreClient.cpp"
	"../Source/ErlPacker.cpp" "../Source/EventManager.cpp"
	"../Source/FoundationEntities.cpp" "../Source/GuildEntities.cpp"
	"../Source/GuildMemberEntities.cpp" "../Source/GuildScheduledEventEntities.cpp"
	"../Source/Http.cpp" "../Source/InputEvents.cpp"
	"../Source/InteractionEntities.cpp" "../Source/JSONIfier.cpp"
	"../Source/MessageEntities.cpp" "../Source/ReactionEntities.cpp"
	"../Source/RoleEntities.cpp" "../Source/SongAPI.cpp"
	"../Source/SoundCloudAPI.cpp" "../Source/SSLClients.cpp"
	"../Source/StageInstanceEntities.cpp" "../Source/StickerEntities.cpp"
	"../Source/ThreadEntities.cpp" "../Source/ThreadPool.cpp"
	"../Source/UserEntities.cpp" "../Source/VoiceConnection.cpp"
	"../Source/WebHookEntities.cpp" "../Source/WebSocketEntities.cpp"
	"../Source/YouTubeAPI.cpp"
)

target_link_libraries(DiscordCoreAPI-Bot PUBLIC OpenSSL::SSL)
target_link_libraries(DiscordCoreAPI-Bot PUBLIC OpenSSL::Crypto)
target_link_libraries(DiscordCoreAPI-Bot PUBLIC -lpthread)
target_link_libraries(DiscordCoreAPI-Bot PUBLIC ${FFMPEG_LIBRARIES})
target_link_libraries(DiscordCoreAPI-Bot PUBLIC CURL::libcurl)
target_link_libraries(DiscordCoreAPI-Bot PUBLIC "${VCPKG_INSTALLED_DIR}/lib/libsodium.a")
target_link_libraries(DiscordCoreAPI-Bot PUBLIC "${VCPKG_INSTALLED_DIR}/lib/libglib-2.0.a")
target_link_libraries(DiscordCoreAPI-Bot PUBLIC "${VCPKG_INSTALLED_DIR}/lib/libopus.a")

message(STATUS "Copying a file...")
configure_file("${CMAKE_SOURCE_DIR}/Certs/SoundCloudCert.pem" "/home/$ENV{USER}/SSL/Certs/SoundCloudCert.pem" COPYONLY)
message(STATUS "Copying a file...")
configure_file("${CMAKE_SOURCE_DIR}/Certs/GoogleCert.pem" "/home/$ENV{USER}/SSL/Certs/GoogleCert.pem" COPYONLY)
message(STATUS "Copying a file...")
configure_file("${CMAKE_SOURCE_DIR}/Certs/DiscordCert.pem" "/home/$ENV{USER}/SSL/Certs/DiscordCert.pem" COPYONLY)
elseif(WIN32)
set(ENV{CMAKE_TOOLCHAIN_FILE} "${VCPKG_BASE_DIR}/scripts/buildsystems/vcpkg.cmake")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../Windows/")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../Windows/")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../Windows/")
set(VCPKG_INSTALLED_DIR "${VCPKG_BASE_DIR}/installed/x64-windows")
set(CMAKE_CXX_STANDARD 23)
if(NOT ${CMAKE_BUILD_TYPE} STREQUAL "Debug")
add_compile_definitions(/O2)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED True)
include_directories("../Include")
include_directories("../Source")
include_directories("${VCPKG_INSTALLED_DIR}/lib/glib-2.0/include")
include_directories("${VCPKG_INSTALLED_DIR}/include/glib-2.0/")
include_directories("${VCPKG_INSTALLED_DIR}/include")
add_subdirectory(Windows-DLL)

add_executable(DiscordCoreAPI-Bot "main.cpp")

target_link_libraries(DiscordCoreAPI-Bot PUBLIC DiscordCoreAPI-DLL)

message(STATUS "Copying a file...")
configure_file("${VCPKG_INSTALLED_DIR}/bin/zlib1.dll" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}/zlib1.dll" COPYONLY)
message(STATUS "Copying a file...")
configure_file("${VCPKG_INSTALLED_DIR}/bin/libcurl.dll" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}/libcurl.dll" COPYONLY)
message(STATUS "Copying a file...")
configure_file("${CMAKE_SOURCE_DIR}/Certs/SoundCloudCert.pem" "C:/SSL/Certs/SoundCloudCert.pem" COPYONLY)
message(STATUS "Copying a file...")
configure_file("${CMAKE_SOURCE_DIR}/Certs/GoogleCert.pem" "C:/SSL/Certs/GoogleCert.pem" COPYONLY)
message(STATUS "Copying a file...")
configure_file("${CMAKE_SOURCE_DIR}/Certs/DiscordCert.pem" "C:/SSL/Certs/DiscordCert.pem" COPYONLY)
endif()