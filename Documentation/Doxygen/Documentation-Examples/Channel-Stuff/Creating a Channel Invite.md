Creating a Channel Invite {#creatingchannelinvite}
============
- Execute the `DiscordCoreAPI::Channels::createChannelInviteAsync()` function, while passing in a data structure of type `DiscordCoreAPI::CreateChannelInviteData`, with a return value of `auto` or `DiscordCoreAPI::InviteData`.
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

		virtual void execute(DiscordCoreAPI::BaseFunctionArguments& args) {
			try {
				DiscordCoreAPI::CreateChannelInviteData dataPackage;
				dataPackage.channelId = args.eventData.getChannelId();
				dataPackage.targetType = DiscordCoreAPI::InviteTargetTypes::Embedded_Application;
				dataPackage.temporary = true;

				DiscordCoreAPI::InviteData channelInvite = Channels::createChannelInviteAsync(dataPackage).get();
				cout << "CHANNEL INVITE: " << channelInvite.inviter.userName << endl;

			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
