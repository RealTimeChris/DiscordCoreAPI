### **Creating a Guild Emoji:**
---
- Access within the `DiscordCoreAPI` namespace, the `Reactions` class.
- Populate a data structure of type `DiscordCoreAPI::CreateGuildEmojiData`.
- Select, from the `Reactions` class, the `createGuildEmojiAsync()` function and execute it, with a return value of type `auto` or `EmojiData`, while passing it the created data structure.
- Call the function with `.get()` added to the end in order to wait for the results now.

```cpp
/// Test.hpp - Header for the "test" command.
/// https://github.com/RealTimeChris/DiscordCoreAPI

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

		std::unique_ptr<BaseFunction> create() {
			return std::make_unique<Test>();
		}

		virtual CoRoutine<void> executeAsync(shared_ptr<DiscordCoreAPI::BaseFunctionArguments> args) {
			try {
				CreateGuildEmojiData dataPackage{};
				dataPackage.guildId = args->eventData.getGuildId();
				dataPackage.name = "TESTEMOJI";
				dataPackage.type = ImageType::JPG;
				vector<unsigned __int8> testImageData{};
				dataPackage.imageData = testImageData;
				auto emoji = Reactions::createGuildEmojiAsync(dataPackage).get();

				cout << "THE NAME: " << emoji.name << endl;

				return;
			}
			catch (...) {
				rethrowException("Test::executeAsync Error: ");
			}
		}
	};
}
#endif
```
