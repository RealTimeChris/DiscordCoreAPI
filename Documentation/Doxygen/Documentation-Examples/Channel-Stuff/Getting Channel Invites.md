Getting Channel Invites {#gettingchannelinvites}
============
- Execute the `DiscordCoreAPI::Channels::getChannelInvitesAsync()` function, while passing in a data structure of type `DiscordCoreAPI::GetChannelInvitesData`, with a return value of `auto` or `std::vector<DiscordCoreAPI::InviteData>`.
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
				DiscordCoreAPI::GetChannelInvitesData dataPackage;
				dataPackage.channelId = args.eventData.getChannelId();

				vector<DiscordCoreAPI::InviteData> channelInvites = Channels::getChannelInvitesAsync(dataPackage).get();

				cout << "THE INVITES: " << endl;
				for (auto value: channelInvites) {
					cout << value.inviter.userName << endl;
				}


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
