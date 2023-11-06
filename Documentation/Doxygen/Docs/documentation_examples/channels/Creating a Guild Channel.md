Creating a Guild Channel {#creatingguildchannel}
============
- Execute the, `channels::createGuildChannelAsync()` function, while passing in a data structure of type `create_guild_channel_data`, with a return value of type `auto` or `channel`.
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
				create_guild_channel_data dataPackage;
				dataPackage.type = channel_type::Guild_Text;
				dataPackage.name = "test channel";
				dataPackage.guildId = args.eventData.getGuildId();
				dataPackage.reason = "testing purposes!";

				vector<channel> channels = channels::getGuildChannelsAsync(const {.guildId = args.eventData.getGuildId()}).get();

				for (const auto& value: channels) {
					if (value.type == channel_type::GUILD_CATEGORY) {
						dataPackage.parentId = value.id;
						break;
					}
				}

				channel channel = channels::createGuildChannelAsync(const& dataPackage).get();

				std::cout << "the name: " << channel.name << std::endl;


			} catch (...) {
				rethrowException("test::execute()");
			}
		}
	};
}
```
