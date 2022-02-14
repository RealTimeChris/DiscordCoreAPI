### **Creating a Guild Sticker:**
---
- Access within the `DiscordCoreAPI` namespace, the `Stickers` class.
- Populate a data structure of type `DiscordCoreAPI::CreateGuildStickerData`.
- Select, from the `Stickers` class, the `createGuildStickerAsync()` function and execute it, with a return value of type `auto` or `Sticker`, while passing it the created data structure.
- Call the function with `.get()` added to the end in order to wait for the results now.

```cpp
// Test.hpp - Header for the "test" command.
// https://github.com/RealTimeChris

#pragma once

#ifndef _TEST_
#define _TEST_

#include "Index.hpp"

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
				CreateGuildStickerData dataPackage{};
				dataPackage.reason = "TESTING PURPOSES!";
				dataPackage.description = "TESTING STICKER";
				dataPackage.guildId = args.eventData.getGuildId();
				dataPackage.name = "TEST STICKER";
				dataPackage.tags = "testing";
				vector<int8_t> fileData{};
				fileData.resize(128 * 128);
				for (int32_t x = 0; x < 128 *128; x += 1) {
					fileData[x] = static_cast<int8_t>(static_cast<float>(x) / static_cast<float>(128 * 128)) * 255;
				}
				dataPackage.file = fileData;

				auto responseData = Stickers::createGuildStickerAsync(dataPackage).get();

				cout << "THE NAME: " << responseData.name << endl;

				co_return;
			}
			catch (...) {
				rethrowException("Test::executeAsync Error: ");
			}
		}

		virtual ~Test() {};
	};
}
#endif
```
