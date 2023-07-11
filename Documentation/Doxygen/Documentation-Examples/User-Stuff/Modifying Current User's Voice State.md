Modifying Current User's Voice State {#modifyingcurrentusersvoicestate}
============
- Execute the, `Users::modifyCurrentUserVoiceStateAsync()` function, while passing in a value of type `ModifyCurrentUserVoiceStateData`, with a return value of type `void`.
- Call the function with `.get()` added to the end in order to wait for the results now.

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

		unique_ptr<BaseFunction> create() {
			return makeUnique<Test>();
		}

		virtual void execute(BaseFunctionArguments& args) {
			try {
				ModifyCurrentUserVoiceStateData dataPackage;
				dataPackage.guildId = args.eventData.getGuildId();
				dataPackage.channelId = args.eventData.getChannelId();
				dataPackage.suppress = false;

				Users::modifyCurrentUserVoiceStateAsync(dataPackage).get();


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
