
### **Getting a Follow-Up Message:**
---
- Access the `Interactions` class of the `DiscordCoreAPI` namespace.
- Select, from the `Interactions` class, the `getFollowUpMessageAsync()` function and execute it, while passing in a data structure of type `DiscordCoreAPI::GetFollowUpMessageData`.

```cpp
/// Test.hpp - Header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#include <Index.hpp>

namespace DiscordCoreAPI {

	class Test : public BaseFunction {
	public:
		Test() {
			this->commandName = "test";
			this->helpDescription = "Testing purposes!";
			EmbedData msgEmbed{};
			msgEmbed.setDescription("------\nSimply enter !test or /test!\n------");
			msgEmbed.setTitle("__**Test Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		std::unique_ptr<BaseFunction> create() {
			return std::make_unique<Test>();
		}

		virtual void executeAsync(unique_ptr<DiscordCoreAPI::BaseFunctionArguments> args) {
			try {
				GetFollowUpMessageData dataPackage{};
				dataPackage.applicationId = getBotUser().id;
				dataPackage.interactionToken = args->eventData.getInteractionToken();
				dataPackage.interactionToken = args->eventData.getMessageId();
				auto responseData = Interactions::getFollowUpMessageAsync(dataPackage);
				return;
			}
			catch (...) {
				rethrowException("Test::executeAsync() Error: ");
			}
		}
	};
}
```
