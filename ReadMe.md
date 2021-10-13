# DiscordCoreAPI
<p align="center">
 <img src="https://github.com/RealTimeChris/DiscordCoreAPI/blob/main/images/TheLogo.png?raw=true"  width="500">
</p>
 
 
## A bot library for Discord, written in C++ using WinRT and a few other useful libraries.

## [Build Instructions](https://github.com/RealTimeChris/DiscordCoreAPI/blob/main/Documentation/Build%20Instructions.md)

## [Doxygen-Generated Documentation](https://realtimechris.github.io/DiscordCoreAPI-Docs/)

## [The Dynamic-Link-Library Distro](https://github.com/RealTimeChris/DiscordCoreAPI-DLL)

## [Discord Server (Where I can be found! As well as some demonstration bots that were built using the DiscordCoreAPI Library.)](https://discord.gg/4JNNXzbH9r)

## [Documentation / Examples](https://github.com/RealTimeChris/DiscordCoreAPI/blob/main/Documentation-Examples.md)

## [Example "Music" Bot Written Using This Library](https://github.com/RealTimeChris/MBot-MusicHouse-Cpp)

## [Example "Game" Bot Written Using This Library](https://github.com/RealTimeChris/MBot-GameHouse-Cpp)

## Features
### Concurrent Discord API Access
- As a result of using the asynchronous agents library, this library has the ability to make fully asynchronous/concurrent requests to the Discord API.

### Select Menus
<p align="left">
 	<img src="https://github.com/RealTimeChris/DiscordCoreAPI/blob/main/images/Screenshot%20(83).png?raw=true"  width="700">
	<img src="https://github.com/RealTimeChris/DiscordCoreAPI/blob/main/images/Screenshot%20(84).png?raw=true"  width="700">
</p>

### User Commands
<p align="left">
 	<img src="https://github.com/RealTimeChris/DiscordCoreAPI/blob/main/images/Screenshot%20(120).png?raw=true"  width="700">
</p>

### Slash Commands and Buttons
<p align="left">
	<img src="https://github.com/RealTimeChris/DiscordCoreAPI/blob/main/images/Screenshot%20(53).png?raw=true"  width="700">
</p>

### A Unified "Input-Event" System
- Both user messages and user interactions are accepted via the `onInputEventCreation` event.
- They can all be responded to using the `DiscordCoreAPI::InputEvents::respondToEvent()` function.
```cpp
EmbedData newEmbed;
newEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
newEmbed.setDescription("------\n__**Sorry, but there's already something playing!**__\n------");
newEmbed.setTimeStamp(getTimeAndDate());
newEmbed.setTitle("__**Playing Issue:**__");
newEmbed.setColor(discordGuild.data.borderColor);
embedsFromSearch.push_back(newEmbed);
if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
	RespondToInputEventData dataPackage(args->eventData);
	dataPackage.type = DesiredInputEventResponseType::RegularMessage;
	dataPackage.addMessageEmbed(newEmbed);
	newEvent = InputEvents::respondToEvent(dataPackage);
	InputEvents::deleteInputEventResponseAsync(newEvent, 20000).get();
}
else {
	RespondToInputEventData dataPackage(args->eventData);
	dataPackage.type = DesiredInputEventResponseType::EphemeralInteractionResponse;
	dataPackage.addMessageEmbed(newEmbed);
	newEvent = InputEvents::respondToEvent(dataPackage);
}
```
## Important Settings
- Under Solution Properties -> General -> C++ Language Standard, is set to "Preview – Features from the Latest C++ Working Draft (/std:c++latest)".
- Under Solution Properties -> Linker -> System -> Stack Reserved Size is set to 8388608.
- [The Precompiled Header](https://github.com/RealTimeChris/DiscordCoreAPI/blob/main/Documentation/Precompiled%20Header.md)
- Under Solution Properties -> VC++ Directories -> External Include Directories, add 'C:\PATH_TO_VCPKG_INSTALLED\installed\x64-windows\lib\glib-2.0\include'.

## Dependencies
- [WinRT](https://docs.microsoft.com/en-us/windows/uwp/cpp-and-winrt-apis/intro-to-using-cpp-with-winrt)

- NOTE: I installed these using the [vcpkg](https://github.com/microsoft/vcpkg) installer.
 
- [Nlohmann Json](https://github.com/nlohmann/json)

- [MongoDB](https://docs.mongodb.com/manual/installation/)

- [LibCurl](https://github.com/curl/curl)

- [LibSodium](https://github.com/jedisct1/libsodium)

- [FFMPEG](http://ffmpeg.org/)

## Roadmap
I am currently working on getting this thing to be "useful", and here are some of the things I plan on implementing:

- Finish adding the base Discord-API data structures, along with classes to utilize/access/change their values.

- Build a few bots with the library and document examples.

If you have any suggestions, feel free to let me know, and I will consider taking you up on it. Thanks!
