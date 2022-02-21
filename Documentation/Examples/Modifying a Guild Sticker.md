### **Modifying a Guild Sticker:**
---
- Access within the `DiscordCoreAPI` namespace, the `Stickers` class.
- Populate a data structure of type `DiscordCoreAPI::ModifyGuildStickerData`.
- Select, from the `Stickers` class, the `modifyGuildStickerAsync()` function and execute it, with a return value of type `auto` or `Sticker`, while passing it the created data structure.
- Call the function with `.get()` added to the end in order to wait for the results now.

```cpp
/// Test.hpp - Header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

#pragma once

#include <Index.hpp>

namespace DiscordCoreAPI {

	class Test : public BaseFunction {
	public:
		Test() {
			this->commandName = "test";
			this->helpDescription = "Testing purposes!";
			EmbedData msgEmbed{};
			msgEmbed.setDescription("------\nSimply enter !test or /test!\n------");
			msgEmbed.setTitle("__**Test Usage:**__");
			msgEmbed.setTimeStamp(getTimeAndDate());
			msgEmbed.setColor("FeFeFe");
			this->helpEmbed = msgEmbed;
		}

		unique_ptr<BaseFunction> create() {
			return make_unique<Test>();
		}

		virtual CoRoutine<void> executeAsync(BaseFunctionArguments args) {
			try {
				GetGuildStickersData dataPackage01{};
				dataPackage01.guildId = args.eventData.getGuildId();
				auto resultVector = Stickers::getGuildStickersAsync(dataPackage01).get();

				ModifyGuildStickerData dataPackage{};
				dataPackage.reason = "TESTING PURPOSES!";
				dataPackage.description = "TESTING STICKER";
				dataPackage.guildId = args.eventData.getGuildId();
				dataPackage.name = "test_sticker";
				dataPackage.tags = "testing";
				dataPackage.stickerId = resultVector[0].id;

				auto responseData = Stickers::modifyGuildStickerAsync(dataPackage).get();

				cout << "THE NAME: " << responseData.name << endl;

				return;
			}
			catch (...) {
				rethrowException("Test::executeAsync Error: ");
			}
		}

		virtual ~Test() = default;
	};
}
```
