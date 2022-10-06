Deleting a Guild Emoji {#deletingaguildemoji}
============
- Execute the, `DiscordCoreAPI::Reactions::deleteGuildEmojiAsync()` function, while passing in a value of type `DiscordCoreAPI::DeleteGuildEmojiData`, with a return value of type `void`.
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

				DiscordCoreAPI::DeleteGuildEmojiData dataPackage;
				dataPackage.guildId = args.eventData.getGuildId();
				dataPackage.emojiId = emojiList[0].id;
				dataPackage.reason = "REASON FOR DELETING!";
				DiscordCoreAPI::Reactions::deleteGuildEmojiAsync(dataPackage).get();


			} catch (...) {
				rethrowException("Test::execute()");
			}
		}
	};
}
```
