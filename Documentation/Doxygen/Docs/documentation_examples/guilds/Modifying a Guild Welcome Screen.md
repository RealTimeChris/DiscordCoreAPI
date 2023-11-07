Modifying a Guild Welcome Screen {#modifying_a_guild_welcome_screen}
============
- Execute the, `discord_core_api::guilds::modifyGuildWelcomeScreenAsync()` function, while passing in a value of type `discord_core_api::modify_guild_welcome_screen_data`, with a return value of type `auto` or `discord_core_api::welcome_screen_data`.
- call the function with `discord_core_api::co_routine::get()` added to the end in order to wait for the results now.

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
				jsonifier::vector<welcome_screen_channel_data> vector;
				welcome_screen_channel_data channelData;
				channelData.channelId = args.eventData.getChannelId();
				channelData.description = "testing channel!";
				channelData.emojiName = "🏳";
				vector.emplace_back(channelData);

				modify_guild_welcome_screen_data& dataPackage01;
				dataPackage01.reason = "testing purposes";
				dataPackage01.guildId = args.eventData.getGuildId();
				dataPackage01.description = "the description!";
				dataPackage01.welcomeChannels = vector;

				auto responseData = guilds::modifyGuildWelcomeScreenAsync(const dataPackage01).get();

				std::cout << boolalpha << responseData.welcomeChannels.at(0).channelId << std::endl;

			} catch (...) {
				rethrowException("test::execute()");
			}
		}
	};
}
```
