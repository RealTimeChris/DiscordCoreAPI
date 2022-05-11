Getting an Invite {#gettinganinvite}
============
- Execute the, `DiscordCoreAPI::Guilds::getInviteAsync()` function, while passing in a value of type `DiscordCoreAPI::GetInviteData`, with a return value of type `auto` or `DiscordCoreAPI::InviteData`.
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
				DiscordCoreAPI::GetGuildInvitesData dataPackage;
				dataPackage.guildId = args.eventData.getGuildId();

				auto responseVector = DiscordCoreAPI::Guilds::getGuildInvitesAsync(dataPackage).get();

				DiscordCoreAPI::GetInviteData dataPackage01;
				dataPackage01.withExpiration = true;
				dataPackage01.withCount = true;
				dataPackage01.inviteId = responseVector[0].code;

				auto responseData = DiscordCoreAPI::Guilds::getInviteAsync(dataPackage01).get();

				cout << "THE CODE: " << responseData.code << endl;


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
