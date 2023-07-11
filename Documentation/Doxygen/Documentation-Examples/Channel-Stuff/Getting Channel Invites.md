Getting Channel Invites {#gettingchannelinvites}
============
- Execute the `Channels::getChannelInvitesAsync()` function, while passing in a data structure of type `GetChannelInvitesData`, with a return value of `auto` or `std::vector<InviteData>`.
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
				GetChannelInvitesData dataPackage;
				dataPackage.channelId = args.eventData.getChannelId();

				vector<InviteData> channelInvites = Channels::getChannelInvitesAsync(dataPackage).get();

				std::cout << "THE INVITES: " << std::endl;
				for (auto value: channelInvites) {
					std::cout << value.inviter.userName << std::endl;
				}


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
