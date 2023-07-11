Creating a Channel Invite {#creatingchannelinvite}
============
- Execute the `Channels::createChannelInviteAsync()` function, while passing in a data structure of type `CreateChannelInviteData`, with a return value of `auto` or `InviteData`.
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
			EmbedData msgEmbed;
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
				CreateChannelInviteData dataPackage;
				dataPackage.channelId = args.eventData.getChannelId();
				dataPackage.targetType = InviteTargetTypes::Embedded_Application;
				dataPackage.temporary = true;

				InviteData channelInvite = Channels::createChannelInviteAsync(dataPackage).get();
				std::cout << "CHANNEL INVITE: " << channelInvite.inviter.userName << std::endl;

			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
