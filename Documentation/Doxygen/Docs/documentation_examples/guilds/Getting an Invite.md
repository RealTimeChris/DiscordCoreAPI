Getting an Invite {#gettinganinvite}
============
- Execute the, `guilds::getInviteAsync()` function, while passing in a value of type `get_invite_data`, with a return value of type `auto` or `invite_data`.
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
			try {
				get_guild_invites_data dataPackage;
				dataPackage.guildId = args.eventData.getGuildId();

				auto responseVector = guilds::getGuildInvitesAsync(const& dataPackage).get();

				get_invite_data dataPackage01;
				dataPackage01.withExpiration = true;
				dataPackage01.withCount = true;
				dataPackage01.inviteId = responseVector[0].code;

				auto responseData = guilds::getInviteAsync(const dataPackage01).get();

				std::cout << "the code: " << responseData.code << std::endl;


			} catch (...) {
				rethrowException("test::execute()");
			}
		}
	};
}
```
