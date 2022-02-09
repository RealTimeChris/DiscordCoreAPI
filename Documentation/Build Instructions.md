# DiscordCoreAPI - Build Instructions
----
- ### Install the [dependencies](https://github.com/RealTimeChris/DiscordCoreAPI#dependencies).
- ### Clone this git repository into a folder.
- ### Set, in CMakeLists.txt, the VCPKG_BASE_DIR to wherever the "lib" and "include" directories of VCPKG are.
- ### Set up a main.cpp like [this one](https://github.com/RealTimeChris/DiscordCoreAPI/blob/main/main.cpp).
- ### Open a terminal inside the git repo's folder.
- ### Run "cmake -S . -B BUILD_TARGET_FOLDER -DCMAKE_BUILD_TYPE=Release_OR_Debug", where BUILD_TARGET_FOLDER is a chosen folder for the solution's build results.
- ### Navigate into the build folder and run the solution to build the executable to use, or run within a terminal "cmake --build ./BUILD_TARGET_FOLDER --config=Release_OR_Debug.
- ### Run the bot! (It will be within a folder named either Windows or Linux, in the repo's top-level folder).
