Getting an Interaction Response {#gettinganinteractionresponse}
============
- Execute the, `Interactions::getInteractionResponseAsync()` function, while passing in a data structure of type `GetInteractionResponseData`, with a return type of `Message`.

```cpp
/// Test.hpp-Header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#include <Index.hpp>

	namespace DiscordCoreAPI {

	class Test : public BaseFunction {
	  public:
		Test() {
			commandName = "test";
			helpDescription = "Testing purposes!";
			EmbedData msgEmbed { };
			msgEmbed.setDescription("------\nSimply enter !test or /test!\n------");
			msgEmbed.setTitle("__**Test Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			helpEmbed = msgEmbed;
		}

		UniquePtr<BaseFunction> create() {
			return makeUnique<Test>();
		}

		virtual void execute(BaseFunctionArguments& args) {
			try {
				GetInteractionResponseData dataPackage;
				dataPackage.applicationId = getBotUser().id;
				dataPackage.interactionToken = args.eventData.getInteractionToken();

				auto interactionResponse = Interactions::getInteractionResponseAsync(dataPackage).get();


			} catch (...) {
				rethrowException("Test::execute() Error: ");
			}
		}
	};
}
```
