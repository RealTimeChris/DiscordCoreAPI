Creating a Guild Sticker {#creatingaguildsticker}
============
- Execute the, `Stickers::createGuildStickerAsync()` function, while passing in a value of type `CreateGuildStickerData`, with a return value of type `auto` or `Sticker`.
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
				CreateGuildStickerData dataPackage;
				dataPackage.reason = "TESTING PURPOSES!";
				dataPackage.description = "TESTING STICKER";
				dataPackage.guildId = args.eventData.getGuildId();
				dataPackage.name = "TEST STICKER";
				dataPackage.tags = "testing";
				vector<uint8_t> fileData { };
				fileData.resize(128 * 128);
				for (int32_t x = 0; x < 128 * 128; ++x) {
					fileData[x] = static_cast<uint8_t>(static_cast<double>(x)/static_cast<double>(128 * 128)) * 255;
				}
				dataPackage.file = fileData;

				auto responseData = Stickers::createGuildStickerAsync(dataPackage).get();

				std::cout << "THE NAME: " << responseData.name << std::endl;


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}

		virtual ~Test() noexcept = default;
	};
}
```
