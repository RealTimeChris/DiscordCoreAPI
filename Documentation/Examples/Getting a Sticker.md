### **Getting a Sticker:**
---
- Access within the `DiscordCoreAPI` namespace, the `Stickers` class.
- Populate a data structure of type `DiscordCoreAPI::GetStickerData`.
- Select, from the `Stickers` class, the `getStickerAsync()` function and execute it, with a return value of type `auto` or `Sticker`, while passing it the created data structure.
- Call the function with `.get()` added to the end in order to wait for the results now.

```cpp
// Test.hpp - Header for the "test" command.
// https://github.com/RealTimeChris

#pragma once

#ifndef _TEST_
#define _TEST_

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
				GetStickerData dataPackage{};
				dataPackage.stickerId = "YOUR_STICKER_ID_HERE";

				auto responseData = Stickers::getStickerAsync(dataPackage).get();

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
