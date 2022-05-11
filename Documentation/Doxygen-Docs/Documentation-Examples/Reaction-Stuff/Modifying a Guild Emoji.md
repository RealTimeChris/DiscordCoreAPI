Modifying a Guild Emoji {#modifyingaguildemoji}
============
- Execute the, `DiscordCoreAPI::Reactions::modifyGuildEmojiAsync()` function, while passing in a value of type `DiscordCoreAPI::ModifyGuildEmojiData`, with a return value of type `auto` or `DiscordCoreAPI::EmojiData`.
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
				DiscordCoreAPI::GetEmojiListData dataPackage00 { };
				dataPackage00.guildId = args.eventData.getGuildId();

				auto emojiList = DiscordCoreAPI::Reactions::getEmojiListAsync(dataPackage00).get();

				DiscordCoreAPI::ModifyGuildEmojiData dataPackage;
				dataPackage.guildId = args.eventData.getGuildId();
				dataPackage.emojiId = emojiList[0].id;
				dataPackage.reason = "TESTING PURPOSES";
				dataPackage.name = "TESTEMOJI";
				auto emoji = DiscordCoreAPI::Reactions::modifyGuildEmojiAsync(dataPackage).get();

				cout << "THE NAME: " << emoji.name << endl;


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
