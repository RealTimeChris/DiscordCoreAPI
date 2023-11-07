Getting a List of Guild Stickers {#getting_a_list_of_guild_stickers}
============
- Execute the, `discord_core_api::stickers::getGuildStickersAsync()` function, while passing in a value of type `discord_core_api::get_guild_stickers_data`, with a return value of type `auto` or `jsonifier::vector<sticker>`.
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
				get_guild_stickers_data dataPackage;
				dataPackage.guildId = args.eventData.getGuildId();

				auto responseData = stickers::getGuildStickersAsync(const dataPackage).get();

				for (auto& value: responseData) {
					std::cout << "the name: " << value.name << std::endl;
				}


			} catch (...) {
				rethrowException("test::execute()");
			}
		}

		virtual ~test() = default;
	};
}
```
