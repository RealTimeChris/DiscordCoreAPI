Running a Function on a Timer {#runningfunctiontimer}
============
- Create a function who's argument is of type `DiscordCoreAPI::DiscordCoreClient*`, with a return type of `void`.
- Create a data value of type `std::vector<DiscordCoreAPI::RepeatedFunctionData>`.
- Create a data value of type `DiscordCoreAPI::RepeatedFunctionData`, and fill out its members, and stuff it inside the vector. Be sure to set `repeated` to true if you would like the function call to be recurring.
- Pass this vector as a second argument to the `DiscordCoreAPI::DiscordCoreClient` constructor - now the functions it contains will run on a schedule defined by the `intervalInMs` member of the data structure.
- Do this with as many functions as you would like!

```cpp
/// main.cpp-Main entry point.
/// Jun 17, 2021
/// https://github.com/RealTimeChris/DiscordCoreAPI

#include <../Include/Index.hpp>
#include "Commands/CommandsList.hpp"

void onBoot02(DiscordCoreAPI::DiscordCoreClients* thePtr) {
	std::vector<DiscordCoreAPI::ActivityData> activities;
	DiscordCoreAPI::ActivityData activity;
	activity.name = "!help for my commands!";
	activity.type = DiscordCoreAPI::ActivityType::Game;
	activities.emplace_back(activity);
	thePtr->getBotUser().updatePresence({.activities = activities, .status = "online", .afk = false});
}

Int32 main() {
	String botToken = "YOUR_BOT_TOKEN_HERE";
	std::vector<DiscordCoreAPI::RepeatedFunctionData> functionVector;
	DiscordCoreAPI::RepeatedFunctionData function01;
	function01.function = onBoot02;
	function01.intervalInMs = 500;
	function01.repeated = false;
	functionVector.emplace_back(function01);
	DiscordCoreAPI::CacheOptions cacheOptions;
	cacheOptions.cacheGuildMembers = true;
	cacheOptions.cacheChannels = true;
	cacheOptions.cacheGuilds = true;
	cacheOptions.cacheRoles = true;
	cacheOptions.cacheUsers = true;
	auto thePtr = std::make_unique<DiscordCoreAPI::DiscordCoreClient>(botToken, functionVector, cacheOptions);
	thePtr->runBot();
	return 0;
}

```
