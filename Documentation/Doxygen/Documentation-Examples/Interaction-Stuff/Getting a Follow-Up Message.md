Getting a Follow-Up Message {#gettingafollowupmessage}
============
- Execute the, `Interactions::getFollowUpMessageAsync()` function and execute it, while passing in a data structure of type `GetFollowUpMessageData`, with a return value of type `auto` or `Message`.

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
				GetFollowUpMessageData dataPackage;
				dataPackage.applicationId = getBotUser().id;
				dataPackage.interactionToken = args.eventData.getInteractionToken();
				dataPackage.interactionToken = args.eventData.getMessageId();
				auto responseData = Interactions::getFollowUpMessageAsync(dataPackage);

			} catch (...) {
				rethrowException("Test::execute() Error: ");
			}
		}
	};
}
```
