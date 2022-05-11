Deleting a Guild Sticker {#deletingaguildsticker}
============
- Execute the, `DiscordCoreAPI::Stickers::deleteGuildStickerAsync()` function, while passing in a value of type `DiscordCoreAPI::DeleteGuildStickerData`, with a return value of type `void`.
- Call the function with `.get()` added to the end in order to wait for the results now.

```cpp
/// Test.hpp-Header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#include <Index.hpp>

namespace DiscordCoreAPI {

	class Test : public DiscordCoreAPI::BaseFunction {
	  public:
		Test() {
			this->commandName = "test";
			this->helpDescription = "Testing purposes!";
			DiscordCoreAPI::EmbedData msgEmbed { };
			msgEmbed.setDescription("------\nSimply enter !test or /test!\n------");
			msgEmbed.setTitle("__**Test Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		std::unique_ptr<DiscordCoreAPI::BaseFunction> create() {
			return std::make_unique<Test>();
		}

		virtual void execute(DiscordCoreAPI::BaseFunctionArguments& args) {
			try {
				DiscordCoreAPI::GetGuildStickersData dataPackage01;
				dataPackage01.guildId = args.eventData.getGuildId();
				auto resultVector = DiscordCoreAPI::Stickers::getGuildStickersAsync(dataPackage01).get();

				DiscordCoreAPI::DeleteGuildStickerData dataPackage;
				dataPackage.reason = "TESTING PURPOSES!";
				dataPackage.guildId = args.eventData.getGuildId();
				dataPackage.stickerId = resultVector[0].id;

				DiscordCoreAPI::Stickers::deleteGuildStickerAsync(dataPackage).get();


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}

		virtual ~Test() = default;
	};
}
```
