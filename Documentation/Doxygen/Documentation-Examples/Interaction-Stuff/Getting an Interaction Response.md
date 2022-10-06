Getting an Interaction Response {#gettinganinteractionresponse}
============
- Execute the, `DiscordCoreAPI::Interactions::getInteractionResponseAsync()` function, while passing in a data structure of type `DiscordCoreAPI::GetInteractionResponseData`, with a return type of `DiscordCoreAPI::Message`.

```cpp
/// Test.hpp-Header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#include <Index.hpp>

	namespace DiscordCoreAPI {

	class Test : public DiscordCoreAPI::BaseFunction {
	  public:
		Test() {
			this->commandName = "test";
			this->helpDescription = "Testing purposes!";
			DiscordCoreAPI::EmbedData msgEmbed { };
			msgEmbed.setDescription("------\nSimply enter !test or /test!\n------");
			msgEmbed.setTitle("__**Test Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		std::unique_ptr<DiscordCoreAPI::BaseFunction> create() {
			return std::make_unique<Test>();
		}

		virtual void execute(DiscordCoreAPI::BaseFunctionArguments& args) {
			try {
				DiscordCoreAPI::GetInteractionResponseData dataPackage;
				dataPackage.applicationId = getBotUser().id;
				dataPackage.interactionToken = args.eventData.getInteractionToken();

				auto interactionResponse = DiscordCoreAPI::Interactions::getInteractionResponseAsync(dataPackage).get();


			} catch (...) {
				rethrowException("Test::execute() Error: ");
			}
		}
	};
}
```
