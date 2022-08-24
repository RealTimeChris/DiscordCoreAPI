Adding a Command {#addingcommand}
============
- First, create the application commands by running the following command... (which is done by entering "!registerapplicationcommands" while running the bot) with the following commands having been registered: \subpage createglobalcommand
- In a new header for the command (that is stored within the Commands folder) \#include the `<discordcoreapi/Index.hpp>` file.
- Add this header to the `Commands/CommandsList.hpp` header.
```cpp
/// CommandsList.hpp - Commands! All of them!
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#include <Test.hpp>

```
- Create a new class, within the `DiscordCoreAPI` namespace, derived from the `DiscordCoreAPI::BaseFunction` class.
- Set the `commandName`, `helpDescription`, and `helpEmbed` members of the class.
- Add a `create()` function, where the return value is a `unique_ptr` to the class type of the current command.
- Add a `virtual void DiscordCoreAPI::BaseFunction::execute()` function with an argument of type `DiscordCoreAPI::BaseFunctionArguments&`.
- CONTINUED FURTHER DOWN.

```cpp
/// Test.hpp - Header for the "test" command.
/// https://discordcoreapi.com

#pragma once

#include "HelperFunctions.hpp"

namespace DiscordCoreAPI {

	class Test : public DiscordCoreAPI::BaseFunction {
	  public:
		Test() {
			this->commandName = "test";
			this->helpDescription = "Testing purposes!";
			DiscordCoreAPI::EmbedData msgEmbed;
			msgEmbed.setDescription("------\nSimply enter !test or /test!\n------");
			msgEmbed.setTitle("__**Test Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		std::unique_ptr<DiscordCoreAPI::BaseFunction> create() {
			return std::make_unique<Test>();
		}

		virtual void execute(DiscordCoreAPI::BaseFunctionArguments& argsNew) {

			DiscordCoreAPI::RespondToInputEventData dataPackage{ argsNew.eventData };
			dataPackage.setResponseType(DiscordCoreAPI::InputEventResponseType::Interaction_Response);
			dataPackage.addContent("This is a test response!");
			DiscordCoreAPI::InputEvents::respondToInputEventAsync(dataPackage).get();

			return;
		}
		virtual ~Test() noexcept = default;
	};
}// namespace DiscordCoreAPI
```

- Add the command to the library's command list by using the `DiscordCoreAPI::DiscordCoreClient::registerFunction()` function.

```cpp
/// Main.cpp - Main source file..
/// https://discordcoreapi.com

#include <discordcoreapi/Index.hpp>

int32_t main() {
	std::string botToken = "YOUR_BOT_TOKEN_HERE";
	auto thePtr = std::make_unique<DiscordCoreAPI::DiscordCoreClient>(botToken);
	thePtr->registerFunction(std::vector<std::string> {"test"}, std::make_unique<DiscordCoreAPI::Test>());
	thePtr->runBot();
}
```
- Now, when you enter `/test`, the command will respond with a message that responds with "This is a test response!".
