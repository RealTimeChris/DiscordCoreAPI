# DiscordCoreAPI
<p align="center">
 <img src="https://github.com/RealTimeChris/DiscordCoreAPI/blob/main/images/TheLogo.png?raw=true"  width="600">
</p>
 
 
## A Discord library for creating bots, written in C++, using WinRT and a few other useful libraries.

## [Documentation / Examples](https://github.com/RealTimeChris/DiscordCoreAPI/blob/main/Documentation-Examples.md)

## Features
### Concurrent Discord API Access
- As a result of using the asynchronous agents library, this library has the ability to make fully asynchronous requests to the Discord API.

### Slash Commands and Buttons
![](https://github.com/RealTimeChris/DiscordCoreAPI/blob/main/images/Screenshot%20(53).png?raw=true)
### A Unified "Input-Event" System
- Both user messages and user interactions are accepted via the `onInputEventCreation` event.
- They can all be responded to using the `InputEventHandler::respondToEvent()` function.
```C++
if (rolesMsgEmbeds.size() == 0 && itemsMessageEmbeds.size() == 0) {
	string msgString = "Sorry, but we are all out of inventory!";
	EmbedData messageEmbed;
	messageEmbed.setAuthor(args->eventData.getUserName(), args->eventData.getAvatarURL());
	messageEmbed.setColor(discordGuild.data.borderColor);
	messageEmbed.setDescription(msgString);
	messageEmbed.setTimeStamp(getTimeAndDate());
	messageEmbed.setTitle("__**Empty Inventory:**__");
	if (args->eventData.eventType == InputEventType::REGULAR_MESSAGE) {
		ReplyMessageData responseData(args->eventData);
		responseData.embeds.push_back(messageEmbed);
		InputEventData event01 = InputEventManager::respondToEvent(responseData).get();
		InputEventManager::deleteInputEventResponse(event01, 20000).get();
	}
	else if (args->eventData.eventType == InputEventType::SLASH_COMMAND_INTERACTION) {
		CreateInteractionResponseData responseData(args->eventData);
		responseData.data.embeds.push_back(messageEmbed);
		InputEventData event01 = InputEventManager::respondToEvent(responseData).get();
		InputEventManager::deleteInputEventResponse(event01, 20000).get();
	}
	co_await mainThread;
	co_return;
}
```
## Important Settings
- Under Solution Properties -> General -> C++ Language Standard, is set to "Preview – Features from the Latest C++ Working Draft (/std:c++latest)".
- Under Solution Properties -> Linker -> System -> Stack Reserver Size is set to 4194304.

## Dependencies
- [WinRT](https://docs.microsoft.com/en-us/windows/uwp/cpp-and-winrt-apis/intro-to-using-cpp-with-winrt)

- NOTE: I installed these using the [vcpkg](https://github.com/microsoft/vcpkg) installer.
 
- [Nlohmann Json](https://github.com/nlohmann/json)

- [MongoDB](https://docs.mongodb.com/manual/installation/)

- [LibCurl](https://github.com/curl/curl)

## Roadmap
I am currently working on getting this thing to be "useful", and here are some of the things I plan on implementing:

- Finish adding the base Discord-API data structures, along with classes to utilize/access/change their values.

- Add a "music-listening" layer with a usable "voice channel" that includes audio encoding/decoding.

- Build a few bots with the library and document examples.

If you have any suggestions, feel free to let me know, and I will consider taking you up on it. Thanks!
