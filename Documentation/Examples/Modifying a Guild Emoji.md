### **Modifying a Guild Emoji:**
---
- Access within the `DiscordCoreAPI` namespace, the `Reactions` class.
- Populate a data structure of type `DiscordCoreAPI::ModifyGuildEmojiData`.
- Select, from the `Reactions` class, the `modifyGuildEmojiAsync()` function and execute it, with a return value of type `auto` or `EmojiData`, while passing it the created data structure.
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

		std::unique_ptr<BaseFunction> create() {
			return std::make_unique<Test>();
		}

		virtual void executeAsync(unique_ptr<DiscordCoreAPI::BaseFunctionArguments> args) {
			try {
				GetEmojiListData dataPackage00{};
				dataPackage00.guildId = args->eventData.getGuildId();

				auto emojiList = Reactions::getEmojiListAsync(dataPackage00).get();

				ModifyGuildEmojiData dataPackage{};
				dataPackage.guildId = args->eventData.getGuildId();
				dataPackage.emojiId = emojiList.at(0).id;
				dataPackage.reason = "TESTING PURPOSES";
				dataPackage.name = "TESTEMOJI";
				auto emoji = Reactions::modifyGuildEmojiAsync(dataPackage).get();

				cout << "THE NAME: " << emoji.name << endl;

				
			}
			catch (...) {
				rethrowException("Test::executeAsync Error: ");
			}
		}
	};
}
```
