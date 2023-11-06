Adding a Guild Member {#addingaguildmember}
============
- Execute the, `guild_members::addGuildMemberAsync()` function, while passing in a value of type `add_guild_member_data`, with a return value of type `auto` or `guild_member`.
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
				add_guild_member_data dataPackage;
				dataPackage.accessToken = "YOUR_ACCESS_TOKEN_HERE";
				dataPackage.userId = args.eventData.getAuthorId();
				dataPackage.guildId = args.eventData.getGuildId();
				dataPackage.nick = "real_time chris";

				auto guildMember = guild_members::addGuildMemberAsync(const& dataPackage).get();

				std::cout << "the name: " << guildMember.user.userName << std::endl;


			} catch (...) {
				rethrowException("test::execute()");
			}
		}
	};
}
```
