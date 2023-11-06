Adding a Recipient to a Group DM {#addingarecipienttoagroupdm}
============
- Execute the, `users::addRecipientToGroupDMAsync()` function, while passing in a value of type `add_recipient_to_group_dmdata`, with a return value of type `void`.
- call the function with `.get()` added to the end in order to wait for the results now.

```cpp
/// Test.hpp -header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#include <index.hpp>

namespace discord_core_api {

	class test : public base_function {
	  public:
		test() {
			commandName = "test";
			helpDescription = "testing purposes!";
			embed_data msgEmbed { };
			msgEmbed.setDescription("------\nSimply enter !test or /test!\n------");
			msgEmbed.setTitle("__**test usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("fe_fe_fe");
			helpEmbed = msgEmbed;
		}

		unique_ptr<base_function> create() {
			return makeUnique<test>();
		}

		virtual void execute(base_function_arguments& args) {
			try {
				guild_member guildMember = guild_members::getCachedGuildMember(
															  {.guildMemberId = args.eventData.getAuthorId(), .guildId = args.eventData.getGuildId()})
															  .get();

				add_recipient_to_group_dmdata dataPackage;
				dataPackage.userId = args.eventData.getAuthorId();
				dataPackage.channelId = "DM_CHANNEL_ID_HERE";
				dataPackage.token = "YOUR_ACCESS_TOKEN_HERE";
				dataPackage.nick = guildMember.nick;

				users::addRecipientToGroupDMAsync(const& dataPackage).get();


			} catch (...) {
				rethrowException("test::execute()");
			}
		}
	};
}
```
