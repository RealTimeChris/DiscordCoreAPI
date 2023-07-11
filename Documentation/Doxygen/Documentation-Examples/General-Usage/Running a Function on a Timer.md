Running a Function on a Timer {#runningfunctiontimer}
============
- Create a function who's argument is of type `DiscordCoreClient*`, with a return type of `void`.
- Create a data value of type `std::vector<RepeatedFunctionData>`.
- Create a data value of type `RepeatedFunctionData`, and fill out its members, and stuff it inside the vector. Be sure to set `repeated` to true if you would like the function call to be recurring.
- Pass this vector as a member that is a part of the `DiscordCoreClientConfig` structure as an argument to the `DiscordCoreClient` constructor - now the functions it contains will run on a schedule defined by the `intervalInMs` member of the data structure.
- Do this with as many functions as you would like!

```cpp
// main.cpp - Main entry point.
// Jun 17, 2021
// RealTimeChris
// https://github.com/RealTimeChris

#include "Commands/CommandsList.hpp"

void onBoot00(DiscordCoreClient* args) {
	auto botUser = args->getBotUser();
	DatabaseManagerAgent::initialize(botUser.id);
	DiscordUser user{ botUser.userName, botUser.id };
}

int32_t main() {
	std::vector<RepeatedFunctionData> functionVector{};
	RepeatedFunctionData function01{};
	function01.function = &onBoot00;
	function01.intervalInMs = 150;
	function01.repeated = false;
	functionVector.push_back(function01);
	DiscordCoreClientConfig clientConfig{};
	clientConfig.functionsToExecute = functionVector;
	auto ptr = makeUnique<DiscordCoreClient>(clientConfig);
	ptr->runBot();
	return 0;
}

```
