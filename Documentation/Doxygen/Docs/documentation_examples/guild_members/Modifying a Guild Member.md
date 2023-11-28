Modifying a Guild Member {#modifying_a_guild_member}
============
- Execute the, from the `discord_core_api::guild_members::modifyGuildMemberAsync()` function, while passing in a value of type `discord_core_api::modify_guild_member_data`, with a return value of type `discord_core_api::guild_member_data`.
- call the function with `discord_core_api::co_routine::get()` added to the end in order to wait for the results now.

```cpp
/// Test.hpp -header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#include "index.hpp"

namespace discord_core_api {

	class test : public base_function {
	  public:
		test() {
			commandName = "test";
			helpDescription = "testing purposes!";
			embed_data msgEmbed;
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
			modify_guild_member_data& dataPackage01;
			dataPackage01.currentChannelId = "";
			dataPackage01.deaf = false;
			dataPackage01.mute = false;
			dataPackage01.newVoiceChannelId = "";
			dataPackage01.nick = "test nick";
			dataPackage01.roleIds = vector<string> {""};
			dataPackage01.guildId = args.eventData.getGuildId();
			dataPackage01.guildMemberId = args.eventData.getAuthorId();

			guild_member_data guildMember01 = guild_members::modifyGuildMemberAsync(dataPackage01).get();
		}
	};
}
```
