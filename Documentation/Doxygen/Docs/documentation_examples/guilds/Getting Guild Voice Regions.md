Getting Guild Voice Regions {#gettingguildvoiceregions}
============
- Execute the, `guilds::getGuildVoiceRegionsAsync()` function, while passing in a value of type `get_guild_voice_regions_data`, with a return value of type `auto` or `jsonifier::vector<voice_region_data>`.
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
				get_guild_voice_regions_data dataPackage01;
				dataPackage01.guildId = args.eventData.getGuildId();

				auto responseData = guilds::getGuildVoiceRegionsAsync(const dataPackage01).get();

				for (const auto& value: responseData) {
					std::cout << value.name << std::endl;
				}


			} catch (...) {
				rethrowException("test::execute()");
			}
		}
	};
}


```
