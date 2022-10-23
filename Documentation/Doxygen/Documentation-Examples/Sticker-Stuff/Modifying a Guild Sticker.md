Modifying a Guild Sticker {#modifyingaguildsticker}
============
- Execute the, `DiscordCoreAPI::Stickers::modifyGuildStickerAsync()` function, while passing in a value of type `DiscordCoreAPI::ModifyGuildStickerData`, with a return value of type `auto` or `DiscordCoreAPI::Sticker`.
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

				DiscordCoreAPI::ModifyGuildStickerData dataPackage;
				dataPackage.reason = "TESTING PURPOSES!";
				dataPackage.description = "TESTING STICKER";
				dataPackage.guildId = args.eventData.getGuildId();
				dataPackage.name = "test_sticker";
				dataPackage.tags = "testing";
				dataPackage.stickerId = resultVector[0].id;

				auto responseData = DiscordCoreAPI::Stickers::modifyGuildStickerAsync(dataPackage).get();

				cout << "THE NAME: " << responseData.name << endl;


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}

		virtual ~Test() noexcept = default;
	};
}
```
