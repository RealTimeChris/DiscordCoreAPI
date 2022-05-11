Modifying a User's Voice State {#modifyingausersvoicestate}
============
- Execute the, `DiscordCoreAPI::Users::modifyUserVoiceStateAsync()` function, while passing in a value of type `DiscordCoreAPI::ModifyUserVoiceStateData`, with a return value of type `void`.
- Call the function with `.get()` added to the end in order to wait for the results now.

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

		unique_ptr<DiscordCoreAPI::BaseFunction> create() {
			return std::make_unique<Test>();
		}

		virtual void execute(DiscordCoreAPI::BaseFunctionArguments& args) {
			try {
				DiscordCoreAPI::ModifyUserVoiceStateData dataPackage;
				dataPackage.guildId = args.eventData.getGuildId();
				dataPackage.channelId = args.eventData.getChannelId();
				dataPackage.suppress = false;
				dataPackage.userId = args.eventData.getAuthorId();

				DiscordCoreAPI::Users::modifyUserVoiceStateAsync(dataPackage).get();


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
