#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "DiscordCoreAPI-DLL" for configuration "Debug"
set_property(TARGET DiscordCoreAPI-DLL APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(DiscordCoreAPI-DLL PROPERTIES
  IMPORTED_IMPLIB_DEBUG "C:/DiscordCoreAPI-DLL/Windows/Debug/DiscordCoreAPI-DLL.lib"
  IMPORTED_LOCATION_DEBUG "C:/DiscordCoreAPI-DLL/Windows/Debug/DiscordCoreAPI-DLL.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS DiscordCoreAPI-DLL )
list(APPEND _IMPORT_CHECK_FILES_FOR_DiscordCoreAPI-DLL "C:/DiscordCoreAPI-DLL/Windows/Debug/DiscordCoreAPI-DLL.lib" "C:/DiscordCoreAPI-DLL/Windows/Debug/DiscordCoreAPI-DLL.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
