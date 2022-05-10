# Set these values, if you are not building with VCPKG.
set(CURL_INCLUDE_DIR "C:/Users/Chris/Downloads/curlNew/include")
set(CURL_RELEASE_ROOT "C:/Users/Chris/Downloads/curlNew/Build/lib/Release")
set(CURL_DEBUG_ROOT "C:/Users/Chris/Downloads/curlNew/Build/lib/Debug")
set(FFMPEG_INCLUDE_DIR "C:/Users/Chris/Downloads/ffmpeg/include")
set(FFMPEG_RELEASE_ROOT "C:/Users/Chris/Downloads/ffmpeg/lib")
set(FFMPEG_DEBUG_ROOT "C:/Users/Chris/Downloads/ffmpeg/lib")
set(NLOHMANN_INCLUDE_DIR "C:/Users/Chris/Downloads/json/include")
set(OPENSSL_INCLUDE_DIR "C:/Users/Chris/Downloads/openssl/include")
set(OPENSSL_RELEASE_ROOT "C:/Users/Chris/Downloads/openssl")
set(OPENSSL_DEBUG_ROOT "C:/Users/Chris/Downloads/openssl")
set(OPUS_INCLUDE_DIR "C:/Users/Chris/Downloads/newopus/include")
set(OPUS_RELEASE_ROOT "C:/Users/Chris/Downloads/newopus/Build/Release")
set(OPUS_DEBUG_ROOT "C:/Users/Chris/Downloads/newopus/Build/Debug")
set(SODIUM_INCLUDE_DIR "C:/Users/Chris/Downloads/libsodium/src/libsodium/include")
set(SODIUM_RELEASE_ROOT "C:/Users/Chris/Downloads/libsodium/bin/x64/Release/v143/dynamic")
set(SODIUM_DEBUG_ROOT "C:/Users/Chris/Downloads/libsodium/bin/x64/Debug/v143/dynamic")

cmake_minimum_required(VERSION 3.20)
set(PROJECT_NAME "DiscordCoreAPI")
set(LIB_NAME "${PROJECT_NAME}")
set(PRODUCT_VERSION "1.0.0")
set(DESCRIPTION "A Discord bot library, written in C++, using custom asynchronous coroutines.")
project(
	"${PROJECT_NAME}"
	VERSION "${PRODUCT_VERSION}"
	LANGUAGES CXX
	HOMEPAGE_URL "https://discordcoreapi.com"
	DESCRIPTION "${DESCRIPTION}"
)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake;${CMAKE_SOURCE_DIR}/CMake/Find-Scripts") 
set(CMAKE_CONFIGURATION_TYPES "${CMAKE_BUILD_TYPE}")

if (NOT EXISTS "${_VCPKG_INSTALLED_DIR}")
	if (WIN32)
		if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
			cmake_path(GET CMAKE_ROOT ROOT_PATH ROOT_DRIVE)
			set(CMAKE_INSTALL_PREFIX "${ROOT_DRIVE}Users/$ENV{USERNAME}/CMake/DiscordCoreAPI" CACHE PATH "The CMake install prefix." FORCE)
		endif()
	elseif(UNIX)
		if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
			set(CMAKE_INSTALL_PREFIX "/usr/local/share/DiscordCoreAPI" CACHE PATH "The CMake install prefix." FORCE)
		endif()
	else()
		message(FATAL_ERROR "Sorry, but your OS is not supported!")
	endif()
	if (NOT EXISTS "${CURL_INCLUDE_DIR}" OR NOT EXISTS "${CURL_RELEASE_ROOT}" OR NOT EXISTS "${CURL_DEBUG_ROOT}"
		OR NOT EXISTS "${FFMPEG_INCLUDE_DIR}" OR NOT EXISTS "${FFMPEG_RELEASE_ROOT}" OR NOT EXISTS "${FFMPEG_DEBUG_ROOT}"
		OR NOT EXISTS "${NLOHMANN_INCLUDE_DIR}" 
		OR NOT EXISTS "${OPENSSL_INCLUDE_DIR}" OR NOT EXISTS "${OPENSSL_RELEASE_ROOT}" OR NOT EXISTS "${OPENSSL_DEBUG_ROOT}"
		OR NOT EXISTS "${OPUS_INCLUDE_DIR}" OR NOT EXISTS "${OPUS_RELEASE_ROOT}" OR NOT EXISTS "${OPUS_DEBUG_ROOT}"
		OR NOT EXISTS "${SODIUM_INCLUDE_DIR}" OR NOT EXISTS "${SODIUM_RELEASE_ROOT}" OR NOT EXISTS "${SODIUM_DEBUG_ROOT}")
		
		message(FATAL_ERROR "Sorry - but please set the _DEBUG_ROOT, _RELEASE_ROOT AND _INCLUDE_DIR values in CMakeLists.txt!")
	endif()
else()
	string(TOLOWER "${CMAKE_HOST_SYSTEM_NAME}" OS_NAME)
	set(VCPKG_BASE_DIR "${_VCPKG_INSTALLED_DIR}/..")
	set(CMAKE_INSTALL_PREFIX "${VCPKG_BASE_DIR}/packages/discordcoreapi_x64-${OS_NAME}")
	cmake_path(GET CMAKE_INSTALL_PREFIX ROOT_PATH ROOT_DRIVE)
	cmake_path(ABSOLUTE_PATH CMAKE_INSTALL_PREFIX BASE_DIRECTORY "${ROOT_DRIVE}" NORMALIZE OUTPUT_VARIABLE CMAKE_INSTALL_PREFIX)
	set(VCPKG_INSTALLED_DIR "${_VCPKG_INSTALLED_DIR}/x64-${OS_NAME}")
	set(VCPKG_INCLUDE_DIR "${VCPKG_INSTALLED_DIR}/include")
	set(VCPKG_RELEASE_ROOT "${VCPKG_INSTALLED_DIR}/lib")
	set(VCPKG_DEBUG_ROOT "${VCPKG_INSTALLED_DIR}/debug/lib")
	set(CURL_INCLUDE_DIR "${VCPKG_INCLUDE_DIR}")
	set(CURL_RELEASE_ROOT "${VCPKG_RELEASE_ROOT}")
	set(CURL_DEBUG_ROOT "${VCPKG_DEBUG_ROOT}")
	set(FFMPEG_INCLUDE_DIR "${VCPKG_INCLUDE_DIR}")
	set(FFMPEG_RELEASE_ROOT "${VCPKG_RELEASE_ROOT}")
	set(FFMPEG_DEBUG_ROOT "${VCPKG_DEBUG_ROOT}")
	set(NLOHMANN_INCLUDE_DIR "${VCPKG_INCLUDE_DIR}")
	set(OPENSSL_INCLUDE_DIR "${VCPKG_INCLUDE_DIR}")
	set(OPENSSL_RELEASE_ROOT "${VCPKG_RELEASE_ROOT}")
	set(OPENSSL_DEBUG_ROOT "${VCPKG_DEBUG_ROOT}")
	set(OPUS_INCLUDE_DIR "${VCPKG_INCLUDE_DIR}")
	set(OPUS_RELEASE_ROOT "${VCPKG_RELEASE_ROOT}")
	set(OPUS_DEBUG_ROOT "${VCPKG_DEBUG_ROOT}")
	set(SODIUM_INCLUDE_DIR "${VCPKG_INCLUDE_DIR}")
	set(SODIUM_RELEASE_ROOT "${VCPKG_RELEASE_ROOT}")
	set(SODIUM_DEBUG_ROOT "${VCPKG_DEBUG_ROOT}")
endif()

add_subdirectory(Library)
