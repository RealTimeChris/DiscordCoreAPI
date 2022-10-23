Running a Function on a Timer {#runningfunctiontimer}
============
- Create a function who's argument is of type `DiscordCoreAPI::DiscordCoreClient*`, with a return type of `void`.
- Create a data value of type `std::vector<DiscordCoreAPI::RepeatedFunctionData>`.
- Create a data value of type `DiscordCoreAPI::RepeatedFunctionData`, and fill out its members, and stuff it inside the vector. Be sure to set `repeated` to true if you would like the function call to be recurring.
- Pass this vector as a member that is a part of the `DiscordCoreAPI::DiscordCoreClientConfig` structure as an argument to the `DiscordCoreAPI::DiscordCoreClient` constructor - now the functions it contains will run on a schedule defined by the `intervalInMs` member of the data structure.
- Do this with as many functions as you would like!

```cpp
// main.cpp - Main entry point.
// Jun 17, 2021
// RealTimeChris
// https://github.com/RealTimeChris

#include "Commands/CommandsList.hpp"

void onBoot00(DiscordCoreAPI::DiscordCoreClient* args) {
	auto botUser = args->getBotUser();
	DiscordCoreAPI::DatabaseManagerAgent::initialize(botUser.id);
	DiscordCoreAPI::DiscordUser user{ botUser.userName, botUser.id };
}

int32_t main() {
	std::vector<DiscordCoreAPI::RepeatedFunctionData> functionVector{};
	DiscordCoreAPI::RepeatedFunctionData function01{};
	function01.function = &onBoot00;
	function01.intervalInMs = 150;
	function01.repeated = false;
	functionVector.push_back(function01);
	DiscordCoreAPI::DiscordCoreClientConfig clientConfig{};
	clientConfig.functionsToExecute = functionVector;
	auto ptr = std::make_unique<DiscordCoreAPI::DiscordCoreClient>(clientConfig);
	ptr->runBot();
	return 0;
}

```
