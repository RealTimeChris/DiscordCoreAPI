Getting an Invite {#gettinganinvite}
============
- Execute the, `Guilds::getInviteAsync()` function, while passing in a value of type `GetInviteData`, with a return value of type `auto` or `InviteData`.
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
				GetGuildInvitesData dataPackage;
				dataPackage.guildId = args.eventData.getGuildId();

				auto responseVector = Guilds::getGuildInvitesAsync(dataPackage).get();

				GetInviteData dataPackage01;
				dataPackage01.withExpiration = true;
				dataPackage01.withCount = true;
				dataPackage01.inviteId = responseVector[0].code;

				auto responseData = Guilds::getInviteAsync(dataPackage01).get();

				std::cout << "THE CODE: " << responseData.code << std::endl;


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
