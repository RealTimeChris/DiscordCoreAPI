Creating a Guild Sticker {#creating_a_guild_sticker}
============
- Execute the, `discord_core_api::stickers::createGuildStickerAsync()` function, while passing in a value of type `discord_core_api::create_guild_sticker_data`, with a return value of type `auto` or `discord_core_api::sticker`.
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
				create_guild_sticker_data& dataPackage;
				dataPackage.reason = "testing purposes!";
				dataPackage.description = "testing sticker";
				dataPackage.guildId = args.eventData.getGuildId();
				dataPackage.name = "test sticker";
				dataPackage.tags = "testing";
				vector<uint8_t> fileData { };
				fileData.resize(128 * 128);
				for (int32_t x = 0; x < 128 * 128; ++x) {
					fileData[x] = static_cast<uint8_t>(static_cast<double>(x)/static_cast<double>(128 * 128)) * 255;
				}
				dataPackage.file = fileData;

				auto responseData = stickers::createGuildStickerAsync(dataPackage).get();

				std::cout << "the name: " << responseData.name << std::endl;


			} catch (...) {
				rethrowException("test::execute()");
			}
		}

		virtual ~test() = default;
	};
}
```
