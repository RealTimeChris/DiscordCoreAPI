Creating a Guild Sticker {#creatingaguildsticker}
============
- Execute the, `DiscordCoreAPI::Stickers::createGuildStickerAsync()` function, while passing in a value of type `DiscordCoreAPI::CreateGuildStickerData`, with a return value of type `auto` or `DiscordCoreAPI::Sticker`.
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

		virtual Void execute(DiscordCoreAPI::BaseFunctionArguments& args) {
			try {
				DiscordCoreAPI::CreateGuildStickerData dataPackage;
				dataPackage.reason = "TESTING PURPOSES!";
				dataPackage.description = "TESTING STICKER";
				dataPackage.guildId = args.eventData.getGuildId();
				dataPackage.name = "TEST STICKER";
				dataPackage.tags = "testing";
				vector<Uint8> fileData { };
				fileData.resize(128 * 128);
				for (Uint32 x = 0; x < 128 * 128; ++x) {
					fileData[x] = static_cast<Uint8>(static_cast<Float>(x)/static_cast<Float>(128 * 128)) * 255;
				}
				dataPackage.file = fileData;

				auto responseData = DiscordCoreAPI::Stickers::createGuildStickerAsync(dataPackage).get();

				cout << "THE NAME: " << responseData.name << endl;


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}

		virtual ~Test() noexcept = default;
	};
}
```
