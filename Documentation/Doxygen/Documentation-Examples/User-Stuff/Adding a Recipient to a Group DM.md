Adding a Recipient to a Group DM {#addingarecipienttoagroupdm}
============
- Execute the, `Users::addRecipientToGroupDMAsync()` function, while passing in a value of type `AddRecipientToGroupDMData`, with a return value of type `void`.
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

		UniquePtr<BaseFunction> create() {
			return makeUnique<Test>();
		}

		virtual void execute(BaseFunctionArguments& args) {
			try {
				GuildMember guildMember = GuildMembers::getCachedGuildMember(
															  {.guildMemberId = args.eventData.getAuthorId(), .guildId = args.eventData.getGuildId()})
															  .get();

				AddRecipientToGroupDMData dataPackage;
				dataPackage.userId = args.eventData.getAuthorId();
				dataPackage.channelId = "DM_CHANNEL_ID_HERE";
				dataPackage.token = "YOUR_ACCESS_TOKEN_HERE";
				dataPackage.nick = guildMember.nick;

				Users::addRecipientToGroupDMAsync(dataPackage).get();


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
