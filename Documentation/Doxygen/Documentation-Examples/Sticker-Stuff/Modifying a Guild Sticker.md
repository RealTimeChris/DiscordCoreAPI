Modifying a Guild Sticker {#modifyingaguildsticker}
============
- Execute the, `Stickers::modifyGuildStickerAsync()` function, while passing in a value of type `ModifyGuildStickerData`, with a return value of type `auto` or `Sticker`.
- Call the function with `.get()` added to the end in order to wait for the results now.

```cpp
/// Test.hpp-Header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#include <Index.hpp>

namespace DiscordCoreAPI {

	class Test : public BaseFunction {
	  public:
		Test() {
			commandName = "test";
			helpDescription = "Testing purposes!";
			EmbedData msgEmbed { };
			msgEmbed.setDescription("------\nSimply enter !test or /test!\n------");
			msgEmbed.setTitle("__**Test Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			helpEmbed = msgEmbed;
		}

		UniquePtr<BaseFunction> create() {
			return makeUnique<Test>();
		}

		virtual void execute(BaseFunctionArguments& args) {
			try {
				GetGuildStickersData dataPackage01;
				dataPackage01.guildId = args.eventData.getGuildId();
				auto resultVector = Stickers::getGuildStickersAsync(dataPackage01).get();

				ModifyGuildStickerData dataPackage;
				dataPackage.reason = "TESTING PURPOSES!";
				dataPackage.description = "TESTING STICKER";
				dataPackage.guildId = args.eventData.getGuildId();
				dataPackage.name = "test_sticker";
				dataPackage.tags = "testing";
				dataPackage.stickerId = resultVector[0].id;

				auto responseData = Stickers::modifyGuildStickerAsync(dataPackage).get();

				std::cout << "THE NAME: " << responseData.name << std::endl;


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}

		virtual ~Test() noexcept = default;
	};
}
```
