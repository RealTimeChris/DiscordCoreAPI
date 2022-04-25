# DiscordCoreAPI
<p align="center">
 <img src="https://github.com/RealTimeChris/DiscordCoreAPI/blob/main/Documentation/Images/TheLogo.png?raw=true"  width="500">
</p>

Hello, and welcome to DiscordCoreAPI! This is a Discord bot library that leverages custom asynchronous [CoRoutines](https://github.com/RealTimeChris/DiscordCoreAPI/blob/main/Include/discordcoreapi/CoRoutine.hpp), as well as a home-brew set of [Https](https://github.com/RealTimeChris/DiscordCoreAPI/blob/main/Source/Http.cpp#L312), [WebSocket](https://github.com/RealTimeChris/DiscordCoreAPI/blob/main/Source/WebSocketEntities.cpp#L36), and [Datagram](https://github.com/RealTimeChris/DiscordCoreAPI/blob/main/Source/WebSocketEntities.cpp#L981) socket clients - all to deliver the utmost performance and efficiency for your bot. It uses roughly 0.1% of an Intel i7 9750h CPU to stream audio in high quality (Opus @ 48Khz, 16-bit) to a single server.

# Documentation/Examples
[Documentation/Examples](https://discordcoreapi.com/d7/d1a/a05634.html)

# Discord Server
[This is a link to the Discord server!](https://discord.gg/adgMqeBuhP)

# Bot Template
[A template for utilizing this library.](https://github.com/RealTimeChris/Bot-Template-for-DiscordCoreAPI)

# Features   

## Performant
- Thanks to utilizing [Erlang Text Format](https://github.com/RealTimeChris/DiscordCoreAPI/blob/main/Source/ErlPacker.cpp) for websocket transfer, and a pool of [kept-alive HTTP connections](https://github.com/RealTimeChris/DiscordCoreAPI/blob/main/Source/Http.cpp#L293-L296) - this library offers the snappiest responses to your interactions and user input.

## CPU Efficient   
- It only uses about 0.1% of an Intel i7 9750h to stream audio in high quality (Opus 48Khz 16-bit Stereo) to a single server.   

## Entire Discord API Covered   
- All of the Discord API endpoints are covered in this library, including voice communication.   

## Concurrent Discord API Access   
- As a result of using [custom asynchronous coroutines](https://github.com/RealTimeChris/DiscordCoreAPI/blob/main/Include/discordcoreapi/CoRoutine.hpp), this library has the ability to make fully    asynchronous/concurrent requests to the Discord API.   

## Slash Commands and Buttons
<p align="left">
 	<img src="https://github.com/RealTimeChris/DiscordCoreAPI/blob/main/Documentation/Images/Slash-Commands and Buttons.png?raw=true"  width="700">   
</p>

## Select Menus
<p align="left">
 	<img src="https://github.com/RealTimeChris/DiscordCoreAPI/blob/main/Documentation/Images/Select-Menu-01.png?raw=true"  width="700">   
</p>
<p align="left">
 	<img src="https://github.com/RealTimeChris/DiscordCoreAPI/blob/main/Documentation/Images/Select-Menu-02.png?raw=true"  width="700">   
</p>
   
## User Commands   
<p align="left">
 	<img src="https://github.com/RealTimeChris/DiscordCoreAPI/blob/main/Documentation/Images/User-Command.png?raw=true"  width="700">   
</p>
   
## Message Commands   
<p align="left">
 	<img src="https://github.com/RealTimeChris/DiscordCoreAPI/blob/main/Documentation/Images/Message-Command.png?raw=true"  width="700">   
</p>
   
## Modal Text Inputs
<p align="left">
 	<img src="https://github.com/RealTimeChris/DiscordCoreAPI/blob/main/Documentation/Images/Modal-01.png?raw=true"  width="700">   
	<img src="https://github.com/RealTimeChris/DiscordCoreAPI/blob/main/Documentation/Images/Modal-02.png?raw=true"  width="700">   
</p>
   
## A Unified "Input-Event" System
- User interactions (Application Commands, Message Commands, User Commands) are accepted via the `DiscordCoreAPI::EventManager::onInputEventCreation` event.
- They can all be responded to using the `DiscordCoreAPI::InputEvents::respondToEvent()` function.
- Alternatively you can implement your own input-event handling by using the raw `DiscordCoreAPI::EventManager::onInteractionCreation` or `DiscordCoreAPI::EventManager::onMessageCreation` events.
```cpp
DiscordCoreAPI::EmbedData newEmbed{};
newEmbed.setAuthor(args.eventData.getUserName(), args.eventData.getAvatarURL());
newEmbed.setDescription("------\\n__**Sorry, but there's already something playing!**__\\n------");
newEmbed.setTimeStamp(getTimeAndDate());
newEmbed.setTitle("__**Playing Issue:**__");
newEmbed.setColor(discordGuild.data.borderColor);
DiscordCoreAPI::RespondToInputEventData dataPackage{ args.eventData };
dataPackage.addMessageEmbed(newEmbed);
dataPackage.setResponseType(DiscordCoreAPI::InputEventResponseType::Ephemeral_Interaction_Response);
DiscordCoreAPI::InputEventData newEvent = DiscordCoreAPI::InputEvents::respondToEvent(dataPackage);
DiscordCoreAPI::InputEvents::deleteInputEventResponseAsync(newEvent, 20000).get();
```

# Supported Platforms
- This library currently supports being built within Visual Studio 2022/MSVC on the Windows platform as well as g++-11 on the Linux platform.

# Build Instructions (Full-Vcpkg)
- [Install vcpkg](https://vcpkg.io/en/getting-started.html), if need be.
- Enter within a terminal `vcpkg install discordcoreapi:x64-windows_OR_linux`.
- Set up a console project in your IDE and make sure to set the C++ standard to C++20 or later - and include `discordcoreapi/Index.hpp`.
- Build and run!

# Dependencies
- [CMake](https://cmake.org/) (Version CMake 3.22 or greater)
- NOTE: I installed these using the [vcpkg](https://github.com/microsoft/vcpkg) installer.
- [OpenSSL](https://wiki.openssl.org/index.php/Compilation_and_Installation#W64) (.\\vcpkg install openssl:x64-windows/x64-linux)
- [Nlohmann Json](https://github.com/nlohmann/json) (.\\vcpkg install nlohmann-json:x64-windows/x64-linux)
- [LibCurl](https://github.com/curl/curl) (.\\vcpkg install curl:x64-windows/x64-linux)
- [LibSodium](https://github.com/jedisct1/libsodium) (.\\vcpkg install libsodium:x64-windows/x64-linux)
- [FFMPEG](http://ffmpeg.org/) (.\\vcpkg install ffmpeg:x64-windows/x64-linux)
- [Glib](https://github.com/GNOME/glib) (.\\vcpkg install glib:x64-windows/x64-linux)
- [Opus](https://opus-codec.org/development/) (.\\vcpkg install opus:x64-windows/x64-linux)

# Build Instructions (Non-Vcpkg) - The Library   
- Install the [dependencies](https://github.com/RealTimeChris/DiscordCoreAPI/blob/main/ReadMe.md#dependencies).   
- Clone [this](https://github.com/RealTimeChris/DiscordCoreAPI) git repository into a folder.   
- Set, in CMakeLists.txt, the `<DEPENDENCY>_INCLUDE_DIR`, `<DEPENDENCY>_DEBUG_ROOT` and `<DEPENDENCY>_RELEASE_ROOT` paths to wherever each of the respective dependencies are located - setting the `<DEPENDENCY>_DEBUG_ROOT` folder to wherever the debug version of the .lib file is, the `<DEPENDENCY>_RELEASE_ROOT` folder to wherever the release version of the .lib file for the library is, and the `<DEPENDENCY>_INCLUDE_DIR` to whichever directory contains the public headers for the library.   
- Open a terminal inside the git repo's folder.   
- Run `cmake -S . --preset Linux_OR_Windows-Debug_OR_Release`.
- Then run `cmake --build --preset Linux_OR_Windows-Debug_OR_Release`.   
- Run within the same terminal and folder `cmake --install ./Build/Debug_OR_Release` (Note: You will likely need to do this from an elevated terminal).   

# The CMAKE Package
- By running `cmake --install ./Build/Debug_OR_Release`, you will be given a cmake package, which can be used to build from this library, using other cmake projects.
- It is used by setting `DiscordCoreAPI_DIR` to wherever the DiscordCoreAPIConfig.cmake file is installed to on your system by running the `cmake --install` command, and then using `find_package()` on `DiscordCoreAPI`.
- When found, you will be granted the following cmake "variables"; `DiscordCoreAPI::DiscordCoreAPI` - this is the library target which can be linked to from other targets in cmake, and on Windows; `$<TARGET_RUNTIME_DLLS:DiscordCoreAPI-Bot>` - which is a list of dll files to be copied into your executable's final location after building.
- [Here](https://github.com/RealTimeChris/Bot-Template-for-DiscordCoreAPI/blob/main/CMakeLists.txt) is an example of building an executable from this library with this method.

# Build Instructions (Non-Vcpkg) - The Executable
- Download the [bot template](https://github.com/RealTimeChris/Bot-Template-for-DiscordCoreAPI) or create your own with the same [imports](https://github.com/RealTimeChris/Bot-Template-for-DiscordCoreAPI/blob/main/CMakeLists.txt#L45), and set within it either the path to the vcpkg base-directory, or the path to the DiscordCoreAPIConfig.cmake file that was created by the installation process before this and the same `<DEPENDENCY>_INCLUDE_DIR`, `<DEPENDENCY>_DEBUG_ROOT` and `<DEPENDENCY>_RELEASE_ROOT` values as before.
- Download, install and run [these following scripts](https://github.com/RealTimeChris/DiscordCoreAPI/tree/main/Library/CMake/Find-Scripts) after the call to project() but before trying to find the DiscordCoreAPI package. Such as [here](https://github.com/RealTimeChris/Bot-Template-for-DiscordCoreAPI/blob/main/Executable/CMakeLists.txt#L12-L20).
- Set up a main.cpp like [this one](https://github.com/RealTimeChris/Bot-Template-for-DiscordCoreAPI/blob/main/main.cpp), including the header `discordcoreapi/Index.hpp`.
- Run in a terminal from within the same folder as the top-level CMakeLists.txt, `cmake -S . --preset Linux_OR_Windows-Debug_OR_Release`.
- Then run `cmake --build --preset Linux_OR_Windows-Debug_OR_Release`.
- If you used the bot template, the resultant executable will be within a folder called `Output Files/Debug_OR_Release`, in the top-level folder of the repo.
	
# Roadmap
I am currently working on getting this thing to be used by people like you! So, if you have any suggestions for the library that would make it more usable - don't hesitate to let me know! I can be
easily found on the Discord server that is linked to above! Cheers and thanks for your time
