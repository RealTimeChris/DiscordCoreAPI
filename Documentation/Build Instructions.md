# DiscordCoreAPI - Build Instructions
----
- ### Install the [dependencies](https://github.com/RealTimeChris/DiscordCoreAPI#dependencies).
- ### Clone this git repository into a folder.
- ### Set the VCPKG_BASE_DIR to wherever the "lib" and "include" directories of VCPKG are, and set the INPUT_DIR either to where it is set (if you're using prebuilt binaries) or to wherever you set the OUTPUT_DIR of the DiscordCoreAPI-DLL CMakeLists.txt to.
- ### Set up a main.cpp like [this one](https://github.com/RealTimeChris/DiscordCoreAPI/blob/main/Documentation/Main.cpp).
- ### Open a terminal inside the git repo's folder.
- ### Run "cmake -S . -B 'BUILD_TARGET_FOLDER'", where 'BUILD_TARGET_FOLDER' is a chosen folder for the solution's build results.
- ### Navigate into the build folder and run the solution to build the executable to use, or run within a terminal "cmake --build ./BUILD_TARGET_FOLDER --config=Release_OR_Debug.
- ### Run the bot!
