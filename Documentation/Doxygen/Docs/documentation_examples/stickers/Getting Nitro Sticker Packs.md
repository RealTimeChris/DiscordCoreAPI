Getting Nitro Sticker Packs {#getting_nitro_sticker_packs}
============
- Execute the, `discord_core_api::stickers::getNitroStickerPacksAsync()` function, with a return value of type `auto` or `jsonifier::vector<sticker_pack_data>`.
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
				auto responseData = stickers::getNitroStickerPacksAsync().get();

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
