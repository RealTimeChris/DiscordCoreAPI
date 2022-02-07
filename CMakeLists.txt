#SET THIS VALUE.
set(VCPKG_BASE_DIR "C:/vcpkg" )

if(UNIX)
	cmake_minimum_required(VERSION 3.16.1)
	project(DiscordCoreAPI-Linux VERSION 1.00 LANGUAGES CXX)
	add_subdirectory(Executable)
elseif(WIN32)
	cmake_minimum_required(VERSION 3.21.2)
	project(DiscordCoreAPI-Windows VERSION 1.00 LANGUAGES CXX)
	add_subdirectory(Executable)
endif()