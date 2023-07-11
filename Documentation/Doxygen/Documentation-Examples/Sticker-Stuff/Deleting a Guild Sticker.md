Deleting a Guild Sticker {#deletingaguildsticker}
============
- Execute the, `Stickers::deleteGuildStickerAsync()` function, while passing in a value of type `DeleteGuildStickerData`, with a return value of type `void`.
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

				DeleteGuildStickerData dataPackage;
				dataPackage.reason = "TESTING PURPOSES!";
				dataPackage.guildId = args.eventData.getGuildId();
				dataPackage.stickerId = resultVector[0].id;

				Stickers::deleteGuildStickerAsync(dataPackage).get();


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}

		virtual ~Test() = default;
	};
}
```
