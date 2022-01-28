# DiscordCoreAPI - Build Instructions
----
- ### Install the [dependencies](https://github.com/RealTimeChris/DiscordCoreAPI#dependencies).
- ### Clone this git repository into a folder.
- ### Set the VcPkgFolder and OpenSSLFolder targets in CMakeLists.txt to be wherever you have vcpkg and OpenSSL installed/built - basically where their "include" and "lib" folders are.
- ### Set up a main.cpp like [this one](https://github.com/RealTimeChris/DiscordCoreAPI/blob/main/Documentation/Main.cpp).
- ### Open a terminal inside the git repo's folder.
- ### Run cmake -S . -B 'BUILD_TARGET_FOLDER', where 'BUILD_TARGET_FOLDER' is a chosen folder for the solution's build results.
- ### Navigate into the build folder and run the solution to build the executable to use.
- ### Run the bot!
