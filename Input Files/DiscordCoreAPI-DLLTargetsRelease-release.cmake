#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "DiscordCoreAPI-DLL" for configuration "Release"
set_property(TARGET DiscordCoreAPI-DLL APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(DiscordCoreAPI-DLL PROPERTIES
  IMPORTED_IMPLIB_RELEASE "C:/DiscordCoreAPI-DLL/Output Files/Release/DiscordCoreAPI-DLL.lib"
  IMPORTED_LOCATION_RELEASE "C:/DiscordCoreAPI-DLL/Output Files/Release/DiscordCoreAPI-DLL.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS DiscordCoreAPI-DLL )
list(APPEND _IMPORT_CHECK_FILES_FOR_DiscordCoreAPI-DLL "C:/DiscordCoreAPI-DLL/Output Files/Release/DiscordCoreAPI-DLL.lib" "C:/DiscordCoreAPI-DLL/Output Files/Release/DiscordCoreAPI-DLL.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
