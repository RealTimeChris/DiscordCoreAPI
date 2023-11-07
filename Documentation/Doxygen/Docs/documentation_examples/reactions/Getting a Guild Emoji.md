Getting a Guild Emoji {#getting_a_guild_emoji}
============
- Execute the, `discord_core_api::reactions::getGuildEmojiAsync()` function, while passing in a value of type `discord_core_api::get_guild_emoji_data`, with a return value of type `auto` or `discord_core_api::emoji_data`.
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
				get_emoji_list_data& dataPackage;
				dataPackage.guildId = args.eventData.getGuildId();

				auto emojiList = reactions::getEmojiListAsync(const& dataPackage).get();

				get_guild_emoji_data& dataPackage01;
				dataPackage01.emojiId = emojiList[0].id;
				dataPackage01.guildId = args.eventData.getGuildId();

				auto emoji = reactions::getGuildEmojiAsync(const& dataPackage).get();

				std::cout << "the name: " << emoji.name << std::endl;


			} catch (...) {
				rethrowException("test::execute()");
			}
		}
	};
}
```
