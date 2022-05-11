Adding a Recipient to a Group DM {#addingarecipienttoagroupdm}
============
- Execute the, `DiscordCoreAPI::Users::addRecipientToGroupDMAsync()` function, while passing in a value of type `DiscordCoreAPI::AddRecipientToGroupDMData`, with a return value of type `void`.
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

		std::unique_ptr<DiscordCoreAPI::BaseFunction> create() {
			return std::make_unique<Test>();
		}

		virtual void execute(DiscordCoreAPI::BaseFunctionArguments& args) {
			try {
				DiscordCoreAPI::GuildMember guildMember = DiscordCoreAPI::GuildMembers::getCachedGuildMemberAsync(
															  {.guildMemberId = args.eventData.getAuthorId(), .guildId = args.eventData.getGuildId()})
															  .get();

				DiscordCoreAPI::AddRecipientToGroupDMData dataPackage;
				dataPackage.userId = args.eventData.getAuthorId();
				dataPackage.channelId = "DM_CHANNEL_ID_HERE";
				dataPackage.token = "YOUR_ACCESS_TOKEN_HERE";
				dataPackage.nick = guildMember.nick;

				DiscordCoreAPI::Users::addRecipientToGroupDMAsync(dataPackage).get();


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
