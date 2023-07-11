Removing a Recipient from a Group DM {#removingarecipientfromagroupdm}
============
- Execute the, `Users::removeRecipientFromGroupDMAsync()` function, while passing in a value of type `RemoveRecipientFromGroupDMData`, with a return value of type `void`.
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

				RemoveRecipientFromGroupDMData dataPackage;
				dataPackage.userId = args.eventData.getAuthorId();
				dataPackage.channelId = "DM_CHANNEL_ID_HERE";

				Users::removeRecipientFromGroupDMAsync(dataPackage).get();


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
